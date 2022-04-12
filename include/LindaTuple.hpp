#pragma once

#include <optional>
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

typedef std::optional<TupleValue> TupleRequirement;

class Tuple
{
  public:
    class Builder
    {
      public:
        Builder();

        Builder &String(std::string &&s);
        Builder &Int(int i);
        Builder &Float(float f);

        Tuple build();

      private:
        std::string             schema_;
        std::vector<TupleValue> values_;
    };

    const std::string             &schema();
    const std::vector<TupleValue> &values();

    std::vector<char> serialize();
    static Tuple      deserialize(const std::vector<char> &data);

  private:
    std::string             schema_;
    std::vector<TupleValue> values_;
};

class TuplePattern
{
  public:
    class Builder
    {
      public:
        Builder();

        Builder &anyString();
        Builder &stringOf(std::string &&s);
        Builder &anyInt();
        Builder &intOf(int i);
        Builder &anyFloat();
        Builder &floatOf(float f);

        TuplePattern build();

      private:
        std::string                   schema_;
        std::vector<TupleRequirement> requirements_;
    };

    const std::string &schema();
    bool               matches(Tuple &tuple);

    std::vector<char>   serialize();
    static TuplePattern deserialize(std::vector<char> &data);

  private:
    std::string                   schema_;
    std::vector<TupleRequirement> requirements_;
};

} // namespace linda