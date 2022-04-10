#include "LindaHandle.hpp"
#include "LindaServer.hpp"
#include "LindaTuple.hpp"

#include <functional>
#include <iostream>
#include <string>

void worker1(linda::Handle handle) {
    std::string  message("Hello example!");
    linda::Tuple t = linda::Tuple::Builder().String(message).build();
    handle.out(t);
}

void worker2(linda::Handle handle) {
    linda::TuplePattern p = linda::TuplePattern::Builder().anyString().build();
    std::cout << std::get<std::string>(handle.read(p)->values()[0]) << std::endl;
}

int main() {
    auto ls = linda::Server(std::vector({std::function(worker1), std::function(worker2)}));
    ls.start();
    return 0;
}
