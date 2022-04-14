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
    linda::TuplePattern tp = linda::TuplePattern::Builder().anyString().intOf(linda::RequirementType::Eq, 1).build();
    linda::Tuple        t  = handle.in(tp).value();
    std::cout << std::get<std::string>(t.values()[0]) << std::endl;
}

int main() {
    auto ls = linda::Server(std::vector({std::function(worker1), std::function(worker2)}));
    ls.start();
    return 0;
}
