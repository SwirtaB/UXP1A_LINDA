#include "LindaHandle.hpp"
#include "LindaServer.hpp"
#include "LindaTuple.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <unistd.h>

// void worker1(linda::Handle handle) {
//     std::cout << "Worker1: Building tuple (\"Hello world!\")" << std::endl;
//     linda::Tuple t = linda::Tuple::Builder().String("Hello example!").build();
//     handle.out(t);
//     handle.close();
// }

// void worker2(linda::Handle handle) {
//     std::cout << "Worker2: Waiting for tuple (string:*)" << std::endl;
//     linda::TuplePattern tp = linda::TuplePattern::Builder().anyString().build();
//     linda::Tuple        t  = handle.in(tp).value();
//     std::cout << "Worker2: got: " << std::get<std::string>(t.values()[0]) << std::endl;
//     handle.close();
// }

void worker1(linda::Handle handle) {
    std::cout << "Worker1: Building tuple (\"Hello world!\")" << std::endl;
    linda::Tuple t = linda::Tuple::Builder().Float(1.0).build();
    handle.out(t);
    handle.close();
}

void worker2(linda::Handle handle) {
    std::cout << "Worker2: Waiting for tuple (string:*)" << std::endl;
    linda::TuplePattern tp = linda::TuplePattern::Builder().floatOf(linda::RequirementType::Eq, 1.0).build();
    linda::Tuple        t  = handle.in(tp).value();
    std::cout << "Worker2: got: " << std::get<float>(t.values()[0]) << std::endl;
    handle.close();
}

int main() {
    auto lindaServer = linda::Server({std::function(worker1), std::function(worker2)});
    lindaServer.start();
    return 0;
}