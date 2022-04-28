#include "LindaHandle.hpp"
#include "LindaServer.hpp"
#include "LindaTuple.hpp"

#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <unistd.h>

void worker1(linda::Handle handle) {
    std::cout << "Worker1: Building tuple ('pi', 3.14159265359, 'e', 2.71828182846)" << std::endl;
    auto tuple = linda::Tuple::Builder().String("pi").Float(3.14159265359).String("e").Float(2.71828182846).build();
    handle.out(tuple);

    std::cout << "Worker1: Sleep for 10 seconds" << std::endl;
    sleep(10);

    std::cout << "Worker1: Waiting to consume tuple ('pi', 3.14159265359, 'e', 2.71828182846)" << std::endl;
    auto tuplePattern = linda::TuplePattern::Builder()
                            .anyString()
                            .floatOf(linda::RequirementTypeFloat::More, 3.0)
                            .anyString()
                            .floatOf(linda::RequirementTypeFloat::Less, 3.0)
                            .build();

    tuple = handle.in(tuplePattern).value();
    std::cout << "Worker1: Consumed tuple " << std::get<std::string>(tuple.values()[0]) << ", "
              << std::get<float>(tuple.values()[1]) << ", " << std::get<std::string>(tuple.values()[2]) << ", "
              << std::get<float>(tuple.values()[3]) << std::endl;

    std::cout << "Worker1: Sleep for 10 seconds" << std::endl;
    sleep(10);

    std::cout << "Worker1: Building tuple ('exit', 0)" << std::endl;
    tuple = linda::Tuple::Builder().String("exit").Int(0).build();
    handle.out(tuple);

    std::cout << "Worker1: Finished" << std::endl;
}
void worker2(linda::Handle handle) {
    auto tuplePattern = linda::TuplePattern::Builder()
                            .anyString()
                            .floatOf(linda::RequirementTypeFloat::More, 3.0)
                            .anyString()
                            .floatOf(linda::RequirementTypeFloat::Less, 3.0)
                            .build();
    std::optional<linda::Tuple> tuple;

    std::cout << "Worker2: Reading tuple ('pi', 3.14159265359, 'e', 2.71828182846) in loop" << std::endl;
    do {
        tuple = handle.read(tuplePattern, 40);
    } while (tuple.has_value());

    std::cout << "Worker2: timedout" << std::endl;
    std::cout << "Worker2: Waiting for tuple (string:exit, int:0)" << std::endl;
    tuplePattern = linda::TuplePattern::Builder()
                       .stringOf(linda::RequirementType::Eq, "exit")
                       .intOf(linda::RequirementType::Eq, 0)
                       .build();
    tuple = handle.read(tuplePattern).value();

    std::cout << "Worker2: Finished" << std::endl;
}
void worker3(linda::Handle handle) {
    auto tuplePattern = linda::TuplePattern::Builder()
                            .anyString()
                            .floatOf(linda::RequirementTypeFloat::More, 3.0)
                            .anyString()
                            .floatOf(linda::RequirementTypeFloat::Less, 3.0)
                            .build();
    std::optional<linda::Tuple> tuple;

    std::cout << "Worker3: Reading tuple ('pi', 3.14159265359, 'e', 2.71828182846) in loop" << std::endl;
    do {
        tuple = handle.read(tuplePattern, 40);
    } while (tuple.has_value());

    std::cout << "Worker3: timedout" << std::endl;
    std::cout << "Worker3: Waiting for tuple (string:exit, int:0)" << std::endl;
    tuplePattern = linda::TuplePattern::Builder()
                       .stringOf(linda::RequirementType::Eq, "exit")
                       .intOf(linda::RequirementType::Eq, 0)
                       .build();
    tuple = handle.read(tuplePattern).value();

    std::cout << "Worker3: Finished" << std::endl;
}
void worker4(linda::Handle handle) {
    auto tuplePattern = linda::TuplePattern::Builder()
                            .anyString()
                            .floatOf(linda::RequirementTypeFloat::More, 3.0)
                            .anyString()
                            .floatOf(linda::RequirementTypeFloat::Less, 3.0)
                            .build();
    std::optional<linda::Tuple> tuple;

    std::cout << "Worker4: Reading tuple ('pi', 3.14159265359, 'e', 2.71828182846) in loop" << std::endl;
    do {
        tuple = handle.read(tuplePattern, 40);
    } while (tuple.has_value());

    std::cout << "Worker4: timedout" << std::endl;
    std::cout << "Worker4: Waiting for tuple (string:exit, int:0)" << std::endl;
    tuplePattern = linda::TuplePattern::Builder()
                       .stringOf(linda::RequirementType::Eq, "exit")
                       .intOf(linda::RequirementType::Eq, 0)
                       .build();
    tuple = handle.read(tuplePattern).value();

    std::cout << "Worker4: Finished" << std::endl;
}

int main() {
    auto ls = linda::Server(
        std::vector({std::function(worker1), std::function(worker2), std::function(worker3), std::function(worker4)}));
    std::cout << "Starting lindaServer" << std::endl;
    ls.start();
    std::cout << "lindaServer closed" << std::endl;
    return 0;
}