#include "../include/LindaServer.hpp"
#include "../include/LindaHandle.hpp"

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <unistd.h>
#include <vector>

namespace linda {

LindaServer::LindaServer(
    const std::vector<std::function<void(LindaHandle)>> workers)
    : workers_(workers){};

int LindaServer::start() {
  for (auto worker : workers_) {
    int in_pipe[2];
    int in_pipe_res = pipe2(in_pipe, O_DIRECT);
    if (in_pipe_res != 0) {
      perror("Failed to construct in pipe");
      return in_pipe_res;
    }

    int out_pipe[2];
    int out_pipe_res = pipe2(out_pipe, O_DIRECT);
    if (out_pipe_res != 0) {
      perror("Failed to construct out pipe");
      return out_pipe_res;
    }

    worker_handles.emplace_back(LindaServer::WorkerHandle{
        .in_pipe = in_pipe[0], .out_pipe = out_pipe[1]});

    if (fork() == 0) {
      worker(LindaHandle(out_pipe[0], in_pipe[1]));
      exit(0);
    } else {
      continue;
    }
  }

  while (true) {
    char read_buff[10000];
    read(worker_handles[0].in_pipe, read_buff, 9999);
    std::cout << read_buff << std::endl;
  }
}

} // namespace linda