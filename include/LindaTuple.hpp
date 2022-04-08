#pragma once

#include <string>
#include <variant>

namespace linda {

enum class TupleType: char {
    String = 's',
    Int = 'i',
    Float = 'f',
};

typedef std::variant<std::string, int, float> TupleValue;

class TuplePattern {
public:

};

class Tuple {
public:
  class Builder {};

  int size();
  TupleType type(int index);

  std::string getString(int index);
  int getInt(int index);
  float getFloat(int index);

private:
};

} // namespace linda