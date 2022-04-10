#pragma once

#include "LindaHandle.hpp"

#include <functional>
#include <vector>

namespace linda
{

class Server
{
  public:
    Server(const std::vector<std::function<void(Handle)>> workers);

    int start();

  private:
    struct WorkerHandle
    {
        int in_pipe;
        int out_pipe;
    };

    const std::vector<std::function<void(Handle)>> workers_;
    std::vector<WorkerHandle>                      worker_handles;

    void spawnWorkers();
    void collectRequests();
    bool completeRequest();
    void waitForRequests();
};

} // namespace linda