#include "LindaHandle.hpp"
#include "LindaServer.hpp"
#include "LindaTuple.hpp"

#include <functional>
#include <iostream>
#include <string>

void worker1(linda::Handle handle) {
    linda::Tuple t = linda::Tuple::Builder().String("Hello example!").Int(1).build();
    handle.out(t);
}

void worker2(linda::Handle handle) {
    linda::TuplePattern p = linda::TuplePattern::Builder().anyString().intOf(1).build();
    std::cout << std::get<std::string>(handle.in(p)->values()[0]) << std::endl;
}

int main() {
    auto ls = linda::Server(std::vector({std::function(worker1), std::function(worker2)}));
    ls.start();
    return 0;
}
