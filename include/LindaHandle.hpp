#pragma once

#include "LindaTuple.hpp"

#include <optional>

namespace linda
{

class Handle
{
  public:
    Handle(int in, int out);

    std::optional<Tuple> read(TuplePattern &pattern, int timeout_ms = 0);
    std::optional<Tuple> in(TuplePattern &pattern, int timeout_ms = 0);
    void                 out(Tuple &tuple);

  private:
    int in_pipe_;
    int out_pipe_;
};

} // namespace linda