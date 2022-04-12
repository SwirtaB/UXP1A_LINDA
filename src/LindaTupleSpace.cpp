#include "LindaTupleSpace.hpp"

#include "LindaTuple.hpp"

namespace linda
{

std::optional<Tuple> TupleSpace::get(TuplePattern &pattern) {
    if (space_.count(pattern.schema())) {
        std::vector<Tuple> &vec = space_.at(pattern.schema());
        for (int i = 0; i < vec.size(); ++i) {
            if (pattern.matches(vec.at(i))) {
                return vec[i];
            }
        }
    }
    return std::nullopt;
}

std::optional<Tuple> TupleSpace::consume(TuplePattern &pattern) {
    if (space_.count(pattern.schema())) {
        std::vector<Tuple> &vec = space_.at(pattern.schema());
        for (int i = 0; i < vec.size(); ++i) {
            if (pattern.matches(vec.at(i))) {
                Tuple t = vec[i];
                vec.erase(vec.begin() + i);
                return t;
            }
        }
    }
    return std::nullopt;
}

void TupleSpace::put(Tuple &tuple) {
    if (!space_.count(tuple.schema())) {
        space_.emplace(tuple.schema(), std::vector<Tuple>());
    }
    space_.at(tuple.schema()).emplace_back(tuple);
}

} // namespace linda