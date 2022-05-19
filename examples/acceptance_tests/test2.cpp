#include "LindaHandle.hpp"
#include "LindaServer.hpp"
#include "LindaTuple.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <unistd.h>

void worker1(linda::Handle handle) {
    std::cout << "Worker1: Building tuple (\"Ala\", \"ma\", 2, \"koty\")" << std::endl;
    auto tuple = linda::Tuple::Builder().String("Ala").String("ma").Int(2).String("koty").build();
    handle.out(tuple);

    std::cout << "Worker1: Waits for (int:1, int:2)" << std::endl;
    auto tuplePattern = linda::TuplePattern::Builder()
                            .intOf(linda::RequirementType::Eq, 1)
                            .intOf(linda::RequirementType::Eq, 2)
                            .build();
    tuple = handle.in(tuplePattern).value();
    std::cout << "Worker1: got: " << std::get<int>(tuple.values()[0]) << ", " << std::get<int>(tuple.values()[1])
              << std::endl;

    std::cout << "Worker1: Waits for (string:'ocena', float:>4.0)" << std::endl;
    tuplePattern = linda::TuplePattern::Builder()
                       .stringOf(linda::RequirementType::Eq, "ocena")
                       .floatOf(linda::RequirementTypeFloat::More, 4.0)
                       .build();
    tuple = handle.in(tuplePattern).value();
    std::cout << "Worker1: got: " << std::get<std::string>(tuple.values()[0]) << ", "
              << std::get<float>(tuple.values()[1]) << std::endl;

    std::cout << "Worker1: Finished" << std::endl;
    std::flush(std::cout);
}

void worker2(linda::Handle handle) {
    std::cout << "Worker2: Waits for (string:*, 'string:ma', int:*, string:'koty')" << std::endl;
    linda::TuplePattern tuplePattern = linda::TuplePattern::Builder()
                                           .anyString()
                                           .stringOf(linda::RequirementType::Eq, "ma")
                                           .anyInt()
                                           .stringOf(linda::RequirementType::Eq, "koty")
                                           .build();
    linda::Tuple tuple = handle.in(tuplePattern).value();
    std::cout << "Worker2: got: " << std::get<std::string>(tuple.values()[0]) << ", "
              << std::get<std::string>(tuple.values()[1]) << ", " << std::get<int>(tuple.values()[2]) << ", "
              << std::get<std::string>(tuple.values()[3]) << std::endl;

    std::cout << "Worker2: Builds 5 tuples (1, 2), (1, 2), (1, 2), (3, 4), (5, 6)" << std::endl;
    for (int i = 0; i < 3; ++i) {
        tuple = linda::Tuple::Builder().Int(1).Int(2).build();
        handle.out(tuple);
    }
    tuple = linda::Tuple::Builder().Int(3).Int(4).build();
    handle.out(tuple);

    tuple = linda::Tuple::Builder().Int(5).Int(6).build();
    handle.out(tuple);

    std::cout << "Worker2: sleeps for 10 seconds" << std::endl;
    sleep(10);

    std::cout << "Worker2: Builds tuple ('ocena', 5.0)" << std::endl;
    tuple = linda::Tuple::Builder().String("ocena").Float(5.0).build();
    handle.out(tuple);

    std::cout << "Worker2: Finished" << std::endl;
    std::flush(std::cout);
}

void worker3(linda::Handle handle) {
    std::cout << "Worker3: Waits for (int:1, int:<3)" << std::endl;
    auto tuplePattern = linda::TuplePattern::Builder()
                            .intOf(linda::RequirementType::Eq, 1)
                            .intOf(linda::RequirementType::Less, 3)
                            .build();
    auto tuple = handle.in(tuplePattern).value();
    std::cout << "Worker3: got: " << std::get<int>(tuple.values()[0]) << ", " << std::get<int>(tuple.values()[1])
              << std::endl;

    std::cout << "Worker3: Finished" << std::endl;
    std::flush(std::cout);
}

void worker4(linda::Handle handle) {
    std::cout << "Worker4: Waits for (int:<=1, int:2)" << std::endl;
    auto tuplePattern = linda::TuplePattern::Builder()
                            .intOf(linda::RequirementType::LessEq, 1)
                            .intOf(linda::RequirementType::Eq, 2)
                            .build();
    auto tuple = handle.in(tuplePattern).value();
    std::cout << "Worker4: got: " << std::get<int>(tuple.values()[0]) << ", " << std::get<int>(tuple.values()[1])
              << std::endl;

    std::cout << "Worker4: Finished" << std::endl;
    std::flush(std::cout);
}

int main() {
    auto ls = linda::Server({worker1, worker2, worker3, worker4});
    std::cout << "Starting lindaServer" << std::endl;
    ls.start();

    std::cout << "lindaServer closed" << std::endl;
    return 0;
}