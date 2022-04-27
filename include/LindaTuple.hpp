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

enum class RequirementTypeSerializable : char
{
    Any    = 0,
    Eq     = 1,
    Less   = 2,
    LessEq = 3,
    More   = 4,
    MoreEq = 5,
};

enum class RequirementType : char
{
    Eq     = 1,
    Less   = 2,
    LessEq = 3,
    More   = 4,
    MoreEq = 5,
};

enum class RequirementTypeFloat : char
{
    Less   = 2,
    LessEq = 3,
    More   = 4,
    MoreEq = 5,
};

typedef std::variant<std::string, int, float> TupleValue;

typedef std::optional<std::pair<RequirementTypeSerializable, TupleValue>> TupleRequirement;

class Tuple
{
  public:
    class Builder
    {
      public:
        Builder();

        Builder &String(const std::string &s);
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
        Builder &stringOf(RequirementType, const std::string &s);
        Builder &anyInt();
        Builder &intOf(RequirementType, int i);
        Builder &anyFloat();
        Builder &floatOf(RequirementTypeFloat, float f);

        TuplePattern build();

      private:
        std::string                   schema_;
        std::vector<TupleRequirement> requirements_;
    };

    const std::string                   &schema();
    const std::vector<TupleRequirement> &requirements();
    bool                                 matches(Tuple &tuple);

    std::vector<char>   serialize();
    static TuplePattern deserialize(const std::vector<char> &data);

  private:
    std::string                   schema_;
    std::vector<TupleRequirement> requirements_;
};

} // namespace linda