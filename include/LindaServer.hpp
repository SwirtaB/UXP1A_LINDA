#pragma once

#include "LindaCommand.hpp"
#include "LindaHandle.hpp"
#include "LindaTupleSpace.hpp"

#include <unordered_map>

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
    std::unordered_map<int, WorkerHandle>          worker_handles_;
    std::vector<std::pair<int, Request>>           requests_;
    std::unordered_map<int, long long>             timeouts_;
    std::chrono::steady_clock                      monotonic_clock_;
    TupleSpace tuple_space_;

    void             spawnWorkers();
    std::vector<int> waitForRequests();
    void             collectRequests(std::vector<int> &ready);
    bool             completeRequest();
    void             timeoutRequests();

    long long getNowMs();
    void      insertTimeout(int fd, int timeout_ms);
    int       findEarliestTimeout();

    void answerRequest(int fd, Response &response);
};

} // namespace linda