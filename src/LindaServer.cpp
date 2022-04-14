#include "LindaServer.hpp"

#include "Debug.hpp"
#include "LindaCommand.hpp"
#include "LindaHandle.hpp"
#include "LindaTuple.hpp"

#include <algorithm>
#include <chrono>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <optional>
#include <poll.h>
#include <ratio>
#include <stdexcept>
#include <sys/poll.h>
#include <unistd.h>
#include <utility>

namespace linda
{

Server::Server(const std::vector<std::function<void(Handle)>> workers) : workers_(workers){}

int Server::start() {
    spawnWorkers();
    server_is_alive_ = true;

    while (server_is_alive_) {
        std::vector<int> ready = waitForRequests();
        if (ready.size() > 0) {
            collectRequests(ready);
            do {
                timeoutRequests();
            } while (completeRequest());
        }
        timeoutRequests();
    }
}

void Server::spawnWorkers() {
    for (auto &worker : workers_) {
        int in_pipe[2];
        int in_pipe_res = pipe(in_pipe);
        if (in_pipe_res != 0) {
            perror("Failed to construct in pipe");
            throw std::runtime_error("Failed to construct in pipe");
        }

        int out_pipe[2];
        int out_pipe_res = pipe(out_pipe);
        if (out_pipe_res != 0) {
            perror("Failed to construct out pipe");
            throw std::runtime_error("Failed to construct out pipe");
        }

        worker_handles_.emplace(in_pipe[0], Server::WorkerHandle{.in_pipe = in_pipe[0], .out_pipe = out_pipe[1]});

        if (fork() == 0) {
            worker(Handle(out_pipe[0], in_pipe[1]));
            exit(0);
        }
    }
}

std::vector<int> Server::waitForRequests() {
    std::vector<pollfd> pollfds;
    for (auto &wh : worker_handles_) {
        pollfd pfd;
        pfd.fd     = wh.second.in_pipe;
        pfd.events = POLLIN;
        pollfds.emplace_back(pfd);
    }

    int timeout  = findEarliestTimeout();
    int poll_res = poll(pollfds.data(), pollfds.size(), timeout);
    if (poll_res < 0) {
        perror("Failed to poll");
        throw std::runtime_error("Failed to poll");
    } else if (poll_res > 0) {
        std::vector<int> ready;
        for (pollfd pfd : pollfds) {
            if (pfd.revents & POLLIN) {
                ready.emplace_back(pfd.fd);
            }
        }
        return ready;
    } else {
        return std::vector<int>();
    }
}

void Server::collectRequests(std::vector<int> &ready) {
    for (int fd : ready) {
        Request request = Request::receive(fd);
        requests_.emplace_back(fd, request);
        if (request.getType() != RequestType::Out) {
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
            std::optional<Response> r = std::nullopt;
            answerRequest(request.first, r);
            return true;
        } else if (type == RequestType::Read) {
            TuplePattern         tp  = request.second.getTuplePattern();
            std::optional<Tuple> get = tuple_space_.get(tp);
            if (get.has_value()) {
                std::optional<Response> r = Response::Result(get.value());
                answerRequest(request.first, r);
                return true;
            }
        } else if (type == RequestType::In) {
            TuplePattern         tp      = request.second.getTuplePattern();
            std::optional<Tuple> consume = tuple_space_.consume(tp);
            if (consume.has_value()) {
                std::optional<Response> r = Response::Result(consume.value());
                answerRequest(request.first, r);
                return true;
            }
        } else if (type == RequestType::Close) {
            worker_handles_.erase(request.first);
            if (worker_handles_.empty()) {
                server_is_alive_ = false;
                return false;
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
        return std::max(earliest - getNowMs(), 0LL);
    }
}

void Server::answerRequest(int fd, std::optional<Response> &response) {
    for (int i = 0; i < requests_.size(); ++i) {
        if (requests_[i].first == fd) {
            requests_.erase(requests_.begin() + i);
            break;
        }
    }
    timeouts_.erase(fd);
    if (response.has_value()) {
        int out_fd = worker_handles_[fd].out_pipe;
        response.value().send(out_fd);
    }
}

} // namespace linda