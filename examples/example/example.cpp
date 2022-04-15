#include "LindaHandle.hpp"
#include "LindaServer.hpp"
#include "LindaTuple.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <unistd.h>

void worker1(linda::Handle handle) {
    linda::Tuple t = linda::Tuple::Builder().String("Hello example!").Int(1).build();
    handle.out(t);
    handle.close();
}

void worker2(linda::Handle handle) {
    linda::TuplePattern tp = linda::TuplePattern::Builder().anyString().intOf(linda::RequirementType::Eq, 1).build();
    linda::Tuple        t  = handle.in(tp).value();
    std::cout << std::get<std::string>(t.values()[0]) << std::endl;
    handle.close();
}

void closingWorker(linda::Handle handle) {
    int sleep_time = 4;
    std::cout << "Closing worker sleeps for " << sleep_time << " seconds..." << std::endl;
    sleep(sleep_time);
    std::cout << "Closing worker closes the Linda Server." << std::endl;
    handle.close();
}

int main() {
    auto ls =
        linda::Server(std::vector({std::function(closingWorker), std::function(worker1), std::function(worker2)}));
    ls.start();
    return 0;
}
