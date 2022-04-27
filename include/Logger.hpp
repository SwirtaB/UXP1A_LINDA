#pragma once

#include "LindaTuple.hpp"

#include <fstream>
#include <string>
namespace linda
{

class Logger
{
  public:
    void           openFile();
    std::ofstream &log();
    std::string    toString(const std::vector<TupleValue>);
    std::string    toString(const std::vector<TupleRequirement> &);
    std::string    to_string(linda::TupleValue value);

  private:
    std::ofstream log_file_;
    std::string   timeNow();
    std::string   dateNow();
};

} // namespace linda