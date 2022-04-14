#include "LindaTupleSpace.hpp"

#include "Debug.hpp"
#include "LindaTuple.hpp"

namespace linda
{

std::optional<Tuple> TupleSpace::get(TuplePattern &pattern) {
    if (space_.count(pattern.schema())) {
        std::vector<Tuple> &vec = space_[pattern.schema()];
        for (int i = 0; i < vec.size(); ++i) {
            if (pattern.matches(vec[i])) {
                return vec[i];
            }
        }
    }
    return std::nullopt;
}

std::optional<Tuple> TupleSpace::consume(TuplePattern &pattern) {
    if (space_.count(pattern.schema())) {
        std::vector<Tuple> &vec = space_[pattern.schema()];
        for (int i = 0; i < vec.size(); ++i) {
            if (pattern.matches(vec[i])) {
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
    space_[tuple.schema()].emplace_back(tuple);
}

} // namespace linda