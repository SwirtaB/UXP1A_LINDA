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
    std::string    toString(const std::vector<TupleValue> &tuple_values);
    std::string    toString(const std::vector<TupleRequirement> &tuple_requirements);
    std::string    toString(const linda::TupleValue &tuple_values);

  private:
    std::ofstream log_file_;
    std::string   timeNow();
    std::string   dateNow();
};

} // namespace linda