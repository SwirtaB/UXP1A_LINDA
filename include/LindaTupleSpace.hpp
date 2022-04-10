#pragma once

#include "LindaTuple.hpp"

#include <optional>
#include <unordered_map>
#include <vector>

namespace linda
{

class TupleSpace
{
  public:
    std::optional<Tuple> get(TuplePattern &pattern);
    std::optional<Tuple> consume(TuplePattern &pattern);
    void                 put(Tuple &tuple);

  private:
    std::unordered_map<std::vector<TupleType>, std::vector<Tuple>> space_;
};
} // namespace linda