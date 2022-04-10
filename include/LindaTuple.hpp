#pragma once

#include <string>
#include <variant>
#include <vector>

namespace linda
{

enum class TupleType : char
{
    String = 's',
    Int    = 'i',
    Float  = 'f',
};

typedef std::variant<std::string, int, float> TupleValue;

typedef std::variant<TupleValue, std::monostate> TupleRequirement;

class Tuple;

class TuplePattern
{
  public:
    class Builder
    {
      public:
        Builder();

        Builder &anyString();
        Builder &stringOf(std::string &s);
        Builder &anyInt();
        Builder &intOf(int i);
        Builder &anyFloat();
        Builder &floatOf(float f);

        TuplePattern build();

      private:
        std::vector<TupleType>        schema_;
        std::vector<TupleRequirement> patterns_;
    };

    const std::vector<TupleType> &schema();
    bool                          matches(Tuple &tuple);

    std::vector<char>   serialize();
    static TuplePattern deserialize(std::vector<char> &data);

  private:
    std::vector<TupleType>        schema_;
    std::vector<TupleRequirement> requirements_;
};

class Tuple
{
  public:
    class Builder
    {
      public:
        Builder();

        Builder &String(std::string &s);
        Builder &Int(int i);
        Builder &Float(float f);

        Tuple build();

      private:
        std::vector<TupleType>  schema_;
        std::vector<TupleValue> values_;
    };

    const std::vector<TupleType>  &schema();
    const std::vector<TupleValue> &values();

    std::vector<char> serialize();
    static Tuple      deserialize(std::vector<char> &data);

  private:
    std::vector<TupleType>  schema_;
    std::vector<TupleValue> values_;
};

} // namespace linda