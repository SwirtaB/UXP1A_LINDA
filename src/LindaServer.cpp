#include "../include/LindaServer.hpp"

#include "../include/LindaHandle.hpp"

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <unistd.h>
#include <vector>

namespace linda
{

Server::Server(const std::vector<std::function<void(Handle)>> workers) : workers_(workers){};

int Server::start() {
    spawnWorkers();
    while (true) {
        collectRequests();
        while (completeRequest())
            ;
        waitForRequests();
    }
}

void Server::spawnWorkers() {
    for (auto worker : workers_) {
        int in_pipe[2];
        int in_pipe_res = pipe2(in_pipe, O_DIRECT);
        if (in_pipe_res != 0) {
            perror("Failed to construct in pipe");
            throw in_pipe_res;
        }

        int out_pipe[2];
        int out_pipe_res = pipe2(out_pipe, O_DIRECT);
        if (out_pipe_res != 0) {
            perror("Failed to construct out pipe");
            throw out_pipe_res;
        }

        worker_handles.emplace_back(Server::WorkerHandle{.in_pipe = in_pipe[0], .out_pipe = out_pipe[1]});

        if (fork() == 0) {
            worker(Handle(out_pipe[0], in_pipe[1]));
            exit(0);
        }
    }
}

void Server::collectRequests() {
    // TODO: collect requests from handles
}

bool Server::completeRequest() {
    // TODO: check if can complete any requests
    // if can, do complete it and return true
    // else return false
    return true;
}

void waitForRequests() {
    // TODO: wait on select or poll for any input from workers
}

} // namespace linda