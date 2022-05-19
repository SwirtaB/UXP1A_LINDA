#include "LindaServer.hpp"

#include "LindaCommand.hpp"
#include "LindaHandle.hpp"
#include "LindaTuple.hpp"

#include <chrono>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <functional>
#include <optional>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/wait.h>
#include <unistd.h>


namespace linda
{

Server::Server(const std::vector<std::function<void(Handle)>> workers) : workers_(workers) {}

int Server::start() {
    logger_.openFile();
    logger_.log() << __FUNCTION__ << "Linda Server starting\n";
    spawnWorkers();

    while (!worker_handles_.empty()) {
        std::vector<int> ready = waitForRequests();
        removeDeadWorkers();
        if (ready.size() > 0) {
            collectRequests(ready);
            do {
                timeoutRequests();
            } while (completeRequest());
        }
        timeoutRequests();
    }
    return 0;
}

void Server::spawnWorkers() {
    for (auto &worker : workers_) {
        int in_pipe[2];
        if (pipe(in_pipe)) {
            perror("Failed to construct in pipe");
            throw std::runtime_error("Failed to construct in pipe");
        }
        int out_pipe[2];
        if (pipe(out_pipe)) {
            perror("Failed to construct out pipe");
            throw std::runtime_error("Failed to construct out pipe");
        }

        int child_pid;

        if ((child_pid = fork()) == 0) {
            if (close(in_pipe[0]) || close(out_pipe[1])) {
                throw std::runtime_error("Server::spawnWorkers - failed to close server fd");
            }
            worker(Handle(out_pipe[0], in_pipe[1]));
            exit(0);
        } else {

            worker_handles_.emplace(
                in_pipe[0], Server::WorkerHandle{.in_pipe = in_pipe[0], .out_pipe = out_pipe[1], .pid = child_pid});

            logger_.log() << __FUNCTION__ << " Spawning worker : in_pipe = " << std::to_string(in_pipe[0])
                          << ", out_pipe = " << std::to_string(out_pipe[1]) << ", pid = " << std::to_string(child_pid)
                          << std::endl;

            if (close(in_pipe[1]) || close(out_pipe[0])) {
                throw std::runtime_error("Server::spawnWorkers - failed to close worker fd");
            }
        }
    }
}

void Server::removeDeadWorkers() {
    std::vector<int> dead;
    for (auto &handle : worker_handles_) {
        int   status;
        pid_t return_pid = waitpid(handle.second.pid, &status, WNOHANG);
        if (return_pid < 0) {
            std::runtime_error("Server::removeDeadWorkers failed to waitpid");
        } else if (return_pid == handle.second.pid) {
            logger_.log() << __FUNCTION__ << " Removing dead worker with pid = " << return_pid << std::endl;
            dead.emplace_back(handle.first);
        }
    }

    for (int fd : dead) {
        std::optional<Response> response = std::nullopt;
        answerRequest(fd, response);
        worker_handles_.erase(fd);
    }
}

std::vector<int> Server::waitForRequests() {
    std::vector<pollfd> pollfds;
    for (auto &wh : worker_handles_) {
        if (!wh.second.closed) {
            pollfd pfd;
            pfd.fd     = wh.second.in_pipe;
            pfd.events = POLLIN;
            pollfds.emplace_back(pfd);
        }
    }

    int timeout  = findEarliestTimeout();
    int poll_res = poll(pollfds.data(), pollfds.size(), timeout);
    if (poll_res < 0) {
        perror("Failed to poll");
        throw std::runtime_error("Failed to poll");
    } else if (poll_res > 0) {
        std::vector<int> ready;
        for (pollfd &pfd : pollfds) {
            if (pfd.revents & POLLERR) {
                logger_.log() << __FUNCTION__ << " Got POLLERR from file descriptor = " << std::to_string(pfd.fd)
                              << std::endl;
                worker_handles_[pfd.fd].closed = true;
            } else if (pfd.revents & POLLHUP) {
                logger_.log() << __FUNCTION__ << " Got POLLHUP from file descriptor = " << std::to_string(pfd.fd)
                              << std::endl;
                worker_handles_[pfd.fd].closed = true;
            } else if (pfd.revents & POLLNVAL) {
                logger_.log() << __FUNCTION__ << " Got POLLINVAL from file descriptor = " << std::to_string(pfd.fd)
                              << std::endl;
                worker_handles_[pfd.fd].closed = true;
            } else if (pfd.revents & POLLIN) {
                logger_.log() << __FUNCTION__ << " Got request from file descriptor = " << std::to_string(pfd.fd)
                              << std::endl;
                ready.emplace_back(pfd.fd);
            }
        }
        return ready;
    } else {
        return std::vector<int>();
    }
}

void Server::collectRequests(const std::vector<int> &ready) {
    for (int fd : ready) {
        logger_.log() << __FUNCTION__ << " Collecting request from file descriptor = " << std::to_string(fd)
                      << std::endl;
        Request request = Request::receive(fd);
        requests_.emplace_back(fd, request);
        if (request.getType() != RequestType::Out) {
            logger_.log() << __FUNCTION__ << " Inserting timeout = " << std::to_string(request.getTimeout())
                          << " to file descriptor = " << std::to_string(fd) << std::endl;
            insertTimeout(fd, request.getTimeout());
        }
    }
}

void Server::timeoutRequests() {
    std::vector<int> timed_out;
    long long        now = getNowMs();

    for (auto &timeout : timeouts_) {
        if (timeout.second <= now) {
            timed_out.emplace_back(timeout.first);
        }
    }
    for (int fd : timed_out) {
        std::optional<Response> r = Response::Timeout();
        logger_.log() << __FUNCTION__ << " Timing out request from file descriptor = " << std::to_string(fd)
                      << std::endl;
        answerRequest(fd, r);
    }
}

bool Server::completeRequest() {
    for (int i = 0; i < requests_.size(); ++i) {
        auto       &request = requests_[i];
        RequestType type    = request.second.getType();
        if (type == RequestType::Out) {
            Tuple t = request.second.getTuple();
            tuple_space_.put(t);
            std::optional<Response> r = Response::Done();
            logger_.log() << __FUNCTION__
                          << " Completing OUT request from file descriptor = " << std::to_string(request.first)
                          << " with schema = " << t.schema() << ", and values = " << logger_.toString(t.values())
                          << std::endl;
            answerRequest(request.first, r);
            return true;
        } else if (type == RequestType::Read) {
            TuplePattern         tp  = request.second.getTuplePattern();
            std::optional<Tuple> get = tuple_space_.get(tp);
            if (get.has_value()) {
                std::optional<Response> r = Response::Result(get.value());
                logger_.log() << __FUNCTION__
                              << " Completing READ request from file descriptor = " << std::to_string(request.first)
                              << " with tuple pattern = " << tp.schema()
                              << " and with requirements = " << logger_.toString(tp.requirements()) << std::endl;
                answerRequest(request.first, r);
                return true;
            }
        } else if (type == RequestType::In) {
            TuplePattern         tp      = request.second.getTuplePattern();
            std::optional<Tuple> consume = tuple_space_.consume(tp);
            if (consume.has_value()) {
                std::optional<Response> r = Response::Result(consume.value());
                logger_.log() << __FUNCTION__
                              << " Completing IN request from file descriptor = " << std::to_string(request.first)
                              << " with tuple pattern = " << tp.schema()
                              << " and with requirements = " << logger_.toString(tp.requirements()) << std::endl;
                answerRequest(request.first, r);
                return true;
            }
        } else {
            throw std::runtime_error("Server::completeRequest - invalid RequestType");
        }
    }
    return false;
}

long long Server::getNowMs() {
    auto m = monotonic_clock_.now().time_since_epoch();
    auto d = std::chrono::duration_cast<std::chrono::milliseconds>(m);
    return d.count();
}

void Server::insertTimeout(int fd, int timeout_ms) {
    if (timeout_ms > 0) {
        long long timeout = getNowMs() + timeout_ms;
        logger_.log() << __FUNCTION__ << " Inserting timeout = " << std::to_string(timeout_ms)
                      << " to file descriptor = " << std::to_string(fd) << std::endl;
        timeouts_.emplace(fd, timeout);
    }
}

int Server::findEarliestTimeout() {
    long long earliest = LONG_LONG_MAX;
    for (auto &timeout : timeouts_) {
        if (timeout.second < earliest) {
            earliest = timeout.second;
        }
    }
    if (earliest == LONG_LONG_MAX) {
        return -1;
    } else {
        logger_.log() << __FUNCTION__ << " Earliest timeout = " << std::to_string(std::max(earliest - getNowMs(), 0LL))
                      << std::endl;
        return std::max(earliest - getNowMs(), 0LL);
    }
}

void Server::answerRequest(int fd, std::optional<Response> &response) {
    for (int i = 0; i < requests_.size(); ++i) {
        if (requests_[i].first == fd) {
            logger_.log() << __FUNCTION__
                          << " Erasing request from requests_ list for file descriptor = " << std::to_string(fd)
                          << std::endl;
            requests_.erase(requests_.begin() + i);
            break;
        }
    }
    timeouts_.erase(fd);
    if (response.has_value()) {
        int out_fd = worker_handles_[fd].out_pipe;

        logger_.log() << __FUNCTION__ << " Answering request from file descriptor = " << std::to_string(fd)
                      << " to file descriptor " << std::to_string(out_fd) << std::endl;

        response.value().send(out_fd);
    }
}

} // namespace linda