#include "LindaHandle.hpp"
#include "LindaServer.hpp"

#include <iostream>
#include <unistd.h>
#include <vector>

void worker(linda::LindaHandle handle) {
    char message1[] = "Hello!";
    char message2[] = "This is an example!";
    write(handle.out_pipe, message1, sizeof(message1));
    write(handle.out_pipe, message2, sizeof(message2));
}

int main() {
    auto ls = linda::LindaServer(std::vector({std::function(worker)}));
    ls.start();
    return 0;
}
