#include "LindaTuple.hpp"

#include "gtest/gtest.h"
#include <string>
#include <variant>

TEST(LINDA_TUPLE_UNIT_TESTS, Tuple_String) {
    // given
    std::string message("hello test");

    // when
    linda::Tuple tuple = linda::Tuple::Builder().String(std::string(message)).build();

    // then
    ASSERT_EQ(tuple.schema(), std::string("s"));
    ASSERT_EQ(tuple.values().size(), 1);
    ASSERT_TRUE(std::holds_alternative<std::string>(tuple.values()[0]));
    ASSERT_EQ(std::get<std::string>(tuple.values()[0]), message);
}

TEST(LINDA_TUPLE_UNIT_TESTS, Tuple_Int) {
    // given
    int value = 1242;

    // when
    linda::Tuple tuple = linda::Tuple::Builder().Int(value).build();

    // then
    ASSERT_EQ(tuple.schema(), std::string("i"));
    ASSERT_EQ(tuple.values().size(), 1);
    ASSERT_TRUE(std::holds_alternative<int>(tuple.values()[0]));
    ASSERT_EQ(std::get<int>(tuple.values()[0]), value);
}

TEST(LINDA_TUPLE_UNIT_TESTS, Tuple_Float) {
    // given
    float value = -123.2;

    // when
    linda::Tuple tuple = linda::Tuple::Builder().Float(value).build();

    // then
    ASSERT_EQ(tuple.schema(), std::string("f"));
    ASSERT_EQ(tuple.values().size(), 1);
    ASSERT_TRUE(std::holds_alternative<float>(tuple.values()[0]));
    ASSERT_EQ(std::get<float>(tuple.values()[0]), value);
}

TEST(LINDA_TUPLE_UNIT_TESTS, Tuple_serialize__to__Tuple_deserialize) {
    // given
    linda::Tuple tuple = linda::Tuple::Builder().Int(12).Float(34).String("test").build();

    // when
    linda::Tuple result = linda::Tuple::deserialize(tuple.serialize());

    // then
    ASSERT_EQ(tuple.schema(), result.schema());
    ASSERT_EQ(tuple.values(), result.values());
    ASSERT_EQ(tuple.serialize(), result.serialize());
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_anyString) {
    // given
    linda::Tuple tuple_a   = linda::Tuple::Builder().String("a").build();
    linda::Tuple tuple_b   = linda::Tuple::Builder().String("b").build();
    linda::Tuple tuple_c   = linda::Tuple::Builder().String("c").build();
    linda::Tuple tuple_int = linda::Tuple::Builder().Int(1).build();

    // when
    linda::TuplePattern pattern = linda::TuplePattern::Builder().anyString().build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("s"));
    ASSERT_TRUE(pattern.matches(tuple_a));
    ASSERT_TRUE(pattern.matches(tuple_b));
    ASSERT_TRUE(pattern.matches(tuple_c));
    ASSERT_FALSE(pattern.matches(tuple_int));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_stringOf__Eq) {
    // given
    linda::Tuple tuple_a   = linda::Tuple::Builder().String("a").build();
    linda::Tuple tuple_b   = linda::Tuple::Builder().String("b").build();
    linda::Tuple tuple_c   = linda::Tuple::Builder().String("c").build();
    linda::Tuple tuple_int = linda::Tuple::Builder().Int(1).build();

    // when
    linda::TuplePattern pattern = linda::TuplePattern::Builder().stringOf(linda::RequirementType::Eq, "b").build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("s"));
    ASSERT_FALSE(pattern.matches(tuple_a));
    ASSERT_TRUE(pattern.matches(tuple_b));
    ASSERT_FALSE(pattern.matches(tuple_c));
    ASSERT_FALSE(pattern.matches(tuple_int));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_stringOf__Less) {
    // given
    linda::Tuple tuple_a   = linda::Tuple::Builder().String("a").build();
    linda::Tuple tuple_b   = linda::Tuple::Builder().String("b").build();
    linda::Tuple tuple_c   = linda::Tuple::Builder().String("c").build();
    linda::Tuple tuple_int = linda::Tuple::Builder().Int(1).build();

    // when
    linda::TuplePattern pattern = linda::TuplePattern::Builder().stringOf(linda::RequirementType::Less, "b").build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("s"));
    ASSERT_TRUE(pattern.matches(tuple_a));
    ASSERT_FALSE(pattern.matches(tuple_b));
    ASSERT_FALSE(pattern.matches(tuple_c));
    ASSERT_FALSE(pattern.matches(tuple_int));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_stringOf__LessEq) {
    // given
    linda::Tuple tuple_a   = linda::Tuple::Builder().String("a").build();
    linda::Tuple tuple_b   = linda::Tuple::Builder().String("b").build();
    linda::Tuple tuple_c   = linda::Tuple::Builder().String("c").build();
    linda::Tuple tuple_int = linda::Tuple::Builder().Int(1).build();

    // when
    linda::TuplePattern pattern = linda::TuplePattern::Builder().stringOf(linda::RequirementType::LessEq, "b").build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("s"));
    ASSERT_TRUE(pattern.matches(tuple_a));
    ASSERT_TRUE(pattern.matches(tuple_b));
    ASSERT_FALSE(pattern.matches(tuple_c));
    ASSERT_FALSE(pattern.matches(tuple_int));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_stringOf__More) {
    // given
    linda::Tuple tuple_a   = linda::Tuple::Builder().String("a").build();
    linda::Tuple tuple_b   = linda::Tuple::Builder().String("b").build();
    linda::Tuple tuple_c   = linda::Tuple::Builder().String("c").build();
    linda::Tuple tuple_int = linda::Tuple::Builder().Int(1).build();

    // when
    linda::TuplePattern pattern = linda::TuplePattern::Builder().stringOf(linda::RequirementType::More, "b").build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("s"));
    ASSERT_FALSE(pattern.matches(tuple_a));
    ASSERT_FALSE(pattern.matches(tuple_b));
    ASSERT_TRUE(pattern.matches(tuple_c));
    ASSERT_FALSE(pattern.matches(tuple_int));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_stringOf__MoreEq) {
    // given
    linda::Tuple tuple_a   = linda::Tuple::Builder().String("a").build();
    linda::Tuple tuple_b   = linda::Tuple::Builder().String("b").build();
    linda::Tuple tuple_c   = linda::Tuple::Builder().String("c").build();
    linda::Tuple tuple_int = linda::Tuple::Builder().Int(1).build();

    // when
    linda::TuplePattern pattern = linda::TuplePattern::Builder().stringOf(linda::RequirementType::MoreEq, "b").build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("s"));
    ASSERT_FALSE(pattern.matches(tuple_a));
    ASSERT_TRUE(pattern.matches(tuple_b));
    ASSERT_TRUE(pattern.matches(tuple_c));
    ASSERT_FALSE(pattern.matches(tuple_int));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_anyInt) {
    // given
    linda::Tuple tuple_1     = linda::Tuple::Builder().Int(1).build();
    linda::Tuple tuple_2     = linda::Tuple::Builder().Int(2).build();
    linda::Tuple tuple_3     = linda::Tuple::Builder().Int(3).build();
    linda::Tuple tuple_float = linda::Tuple::Builder().Float(1.0).build();

    // when
    linda::TuplePattern pattern = linda::TuplePattern::Builder().anyInt().build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("i"));
    ASSERT_TRUE(pattern.matches(tuple_1));
    ASSERT_TRUE(pattern.matches(tuple_2));
    ASSERT_TRUE(pattern.matches(tuple_3));
    ASSERT_FALSE(pattern.matches(tuple_float));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_intOf__Eq) {
    // given
    linda::Tuple tuple_1     = linda::Tuple::Builder().Int(1).build();
    linda::Tuple tuple_2     = linda::Tuple::Builder().Int(2).build();
    linda::Tuple tuple_3     = linda::Tuple::Builder().Int(3).build();
    linda::Tuple tuple_float = linda::Tuple::Builder().Float(1.0).build();

    // when
    linda::TuplePattern pattern = linda::TuplePattern::Builder().intOf(linda::RequirementType::Eq, 2).build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("i"));
    ASSERT_FALSE(pattern.matches(tuple_1));
    ASSERT_TRUE(pattern.matches(tuple_2));
    ASSERT_FALSE(pattern.matches(tuple_3));
    ASSERT_FALSE(pattern.matches(tuple_float));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_intOf__Less) {
    // given
    linda::Tuple tuple_1     = linda::Tuple::Builder().Int(1).build();
    linda::Tuple tuple_2     = linda::Tuple::Builder().Int(2).build();
    linda::Tuple tuple_3     = linda::Tuple::Builder().Int(3).build();
    linda::Tuple tuple_float = linda::Tuple::Builder().Float(1.0).build();

    // when
    linda::TuplePattern pattern = linda::TuplePattern::Builder().intOf(linda::RequirementType::Less, 2).build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("i"));
    ASSERT_TRUE(pattern.matches(tuple_1));
    ASSERT_FALSE(pattern.matches(tuple_2));
    ASSERT_FALSE(pattern.matches(tuple_3));
    ASSERT_FALSE(pattern.matches(tuple_float));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_intOf__LessEq) {
    // given
    linda::Tuple tuple_1     = linda::Tuple::Builder().Int(1).build();
    linda::Tuple tuple_2     = linda::Tuple::Builder().Int(2).build();
    linda::Tuple tuple_3     = linda::Tuple::Builder().Int(3).build();
    linda::Tuple tuple_float = linda::Tuple::Builder().Float(1.0).build();

    // when
    linda::TuplePattern pattern = linda::TuplePattern::Builder().intOf(linda::RequirementType::LessEq, 2).build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("i"));
    ASSERT_TRUE(pattern.matches(tuple_1));
    ASSERT_TRUE(pattern.matches(tuple_2));
    ASSERT_FALSE(pattern.matches(tuple_3));
    ASSERT_FALSE(pattern.matches(tuple_float));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_intOf__More) {
    // given
    linda::Tuple tuple_1     = linda::Tuple::Builder().Int(1).build();
    linda::Tuple tuple_2     = linda::Tuple::Builder().Int(2).build();
    linda::Tuple tuple_3     = linda::Tuple::Builder().Int(3).build();
    linda::Tuple tuple_float = linda::Tuple::Builder().Float(1.0).build();

    // when
    linda::TuplePattern pattern = linda::TuplePattern::Builder().intOf(linda::RequirementType::More, 2).build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("i"));
    ASSERT_FALSE(pattern.matches(tuple_1));
    ASSERT_FALSE(pattern.matches(tuple_2));
    ASSERT_TRUE(pattern.matches(tuple_3));
    ASSERT_FALSE(pattern.matches(tuple_float));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_intOf__MoreEq) {
    // given
    linda::Tuple tuple_1     = linda::Tuple::Builder().Int(1).build();
    linda::Tuple tuple_2     = linda::Tuple::Builder().Int(2).build();
    linda::Tuple tuple_3     = linda::Tuple::Builder().Int(3).build();
    linda::Tuple tuple_float = linda::Tuple::Builder().Float(1.0).build();

    // when
    linda::TuplePattern pattern = linda::TuplePattern::Builder().intOf(linda::RequirementType::MoreEq, 2).build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("i"));
    ASSERT_FALSE(pattern.matches(tuple_1));
    ASSERT_TRUE(pattern.matches(tuple_2));
    ASSERT_TRUE(pattern.matches(tuple_3));
    ASSERT_FALSE(pattern.matches(tuple_float));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_anyFloat) {
    // given
    linda::Tuple tuple_1      = linda::Tuple::Builder().Float(1.0).build();
    linda::Tuple tuple_2      = linda::Tuple::Builder().Float(2.0).build();
    linda::Tuple tuple_3      = linda::Tuple::Builder().Float(3.0).build();
    linda::Tuple tuple_string = linda::Tuple::Builder().String("test").build();

    // when
    linda::TuplePattern pattern = linda::TuplePattern::Builder().anyFloat().build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("f"));
    ASSERT_TRUE(pattern.matches(tuple_1));
    ASSERT_TRUE(pattern.matches(tuple_2));
    ASSERT_TRUE(pattern.matches(tuple_3));
    ASSERT_FALSE(pattern.matches(tuple_string));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_floatOf__Less) {
    // given
    linda::Tuple tuple_1      = linda::Tuple::Builder().Float(1.0).build();
    linda::Tuple tuple_2      = linda::Tuple::Builder().Float(2.0).build();
    linda::Tuple tuple_3      = linda::Tuple::Builder().Float(3.0).build();
    linda::Tuple tuple_string = linda::Tuple::Builder().String("test").build();

    // when
    linda::TuplePattern pattern =
        linda::TuplePattern::Builder().floatOf(linda::RequirementTypeFloat::Less, 2.0).build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("f"));
    ASSERT_TRUE(pattern.matches(tuple_1));
    ASSERT_FALSE(pattern.matches(tuple_2));
    ASSERT_FALSE(pattern.matches(tuple_3));
    ASSERT_FALSE(pattern.matches(tuple_string));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_floatOf__LessEq) {
    // given
    linda::Tuple tuple_1      = linda::Tuple::Builder().Float(1.0).build();
    linda::Tuple tuple_2      = linda::Tuple::Builder().Float(2.0).build();
    linda::Tuple tuple_3      = linda::Tuple::Builder().Float(3.0).build();
    linda::Tuple tuple_string = linda::Tuple::Builder().String("test").build();

    // when
    linda::TuplePattern pattern =
        linda::TuplePattern::Builder().floatOf(linda::RequirementTypeFloat::LessEq, 2.0).build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("f"));
    ASSERT_TRUE(pattern.matches(tuple_1));
    ASSERT_TRUE(pattern.matches(tuple_2));
    ASSERT_FALSE(pattern.matches(tuple_3));
    ASSERT_FALSE(pattern.matches(tuple_string));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_floatOf__More) {
    // given
    linda::Tuple tuple_1      = linda::Tuple::Builder().Float(1.0).build();
    linda::Tuple tuple_2      = linda::Tuple::Builder().Float(2.0).build();
    linda::Tuple tuple_3      = linda::Tuple::Builder().Float(3.0).build();
    linda::Tuple tuple_string = linda::Tuple::Builder().String("test").build();

    // when
    linda::TuplePattern pattern =
        linda::TuplePattern::Builder().floatOf(linda::RequirementTypeFloat::More, 2.0).build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("f"));
    ASSERT_FALSE(pattern.matches(tuple_1));
    ASSERT_FALSE(pattern.matches(tuple_2));
    ASSERT_TRUE(pattern.matches(tuple_3));
    ASSERT_FALSE(pattern.matches(tuple_string));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_floatOf__MoreEq) {
    // given
    linda::Tuple tuple_1      = linda::Tuple::Builder().Float(1.0).build();
    linda::Tuple tuple_2      = linda::Tuple::Builder().Float(2.0).build();
    linda::Tuple tuple_3      = linda::Tuple::Builder().Float(3.0).build();
    linda::Tuple tuple_string = linda::Tuple::Builder().String("test").build();

    // when
    linda::TuplePattern pattern =
        linda::TuplePattern::Builder().floatOf(linda::RequirementTypeFloat::MoreEq, 2.0).build();

    // then
    ASSERT_EQ(pattern.schema(), std::string("f"));
    ASSERT_FALSE(pattern.matches(tuple_1));
    ASSERT_TRUE(pattern.matches(tuple_2));
    ASSERT_TRUE(pattern.matches(tuple_3));
    ASSERT_FALSE(pattern.matches(tuple_string));
}

TEST(LINDA_TUPLE_UNIT_TESTS, TuplePattern_serialize__to__TuplePattern_deserialize) {
    // given
    linda::TuplePattern pattern = linda::TuplePattern::Builder()
                                      .anyInt()
                                      .anyFloat()
                                      .anyString()
                                      .intOf(linda::RequirementType::Eq, 1)
                                      .floatOf(linda::RequirementTypeFloat::LessEq, -12.9)
                                      .stringOf(linda::RequirementType::More, "test")
                                      .build();

    // when
    linda::TuplePattern result = linda::TuplePattern::deserialize(pattern.serialize());

    // then
    ASSERT_EQ(pattern.schema(), result.schema());
    ASSERT_EQ(pattern.serialize(), result.serialize());
}
