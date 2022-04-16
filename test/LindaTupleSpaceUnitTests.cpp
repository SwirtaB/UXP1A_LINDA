#include "LindaTuple.hpp"
#include "LindaTupleSpace.hpp"

#include "gtest/gtest.h"
#include <optional>

TEST(LINDA_TUPLE_SPACE_UNIT_TESTS, TupleSpace_consume__with_no_value) {
    // given
    linda::TupleSpace ts;

    // when
    linda::TuplePattern         pattern = linda::TuplePattern::Builder().anyInt().anyFloat().anyString().build();
    std::optional<linda::Tuple> result  = ts.consume(pattern);

    // then
    ASSERT_FALSE(result.has_value());
}

TEST(LINDA_TUPLE_SPACE_UNIT_TESTS, TupleSpace_consume__with_value) {
    // given
    linda::TupleSpace ts;
    linda::Tuple      tuple = linda::Tuple::Builder().Int(4).Float(-1.2).String("test").build();
    ts.put(tuple);

    // when
    linda::TuplePattern         pattern = linda::TuplePattern::Builder().anyInt().anyFloat().anyString().build();
    std::optional<linda::Tuple> result1 = ts.consume(pattern);
    std::optional<linda::Tuple> result2 = ts.consume(pattern);

    // then
    ASSERT_TRUE(result1.has_value());
    ASSERT_EQ(result1.value().schema(), tuple.schema());
    ASSERT_EQ(result1.value().values(), tuple.values());
    ASSERT_FALSE(result2.has_value());
}

TEST(LINDA_TUPLE_SPACE_UNIT_TESTS, TupleSpace_read__with_no_value) {
    // given
    linda::TupleSpace ts;

    // when
    linda::TuplePattern         pattern = linda::TuplePattern::Builder().anyInt().anyFloat().anyString().build();
    std::optional<linda::Tuple> result  = ts.consume(pattern);

    // then
    ASSERT_FALSE(result.has_value());
}

TEST(LINDA_TUPLE_SPACE_UNIT_TESTS, TupleSpace_read__with_value) {
    // given
    linda::TupleSpace ts;
    linda::Tuple      tuple = linda::Tuple::Builder().Int(4).Float(-1.2).String("test").build();
    ts.put(tuple);

    // when
    linda::TuplePattern         pattern = linda::TuplePattern::Builder().anyInt().anyFloat().anyString().build();
    std::optional<linda::Tuple> result1 = ts.get(pattern);
    std::optional<linda::Tuple> result2 = ts.get(pattern);

    // then
    ASSERT_TRUE(result1.has_value());
    ASSERT_EQ(result1.value().schema(), tuple.schema());
    ASSERT_EQ(result1.value().values(), tuple.values());
    ASSERT_TRUE(result2.has_value());
    ASSERT_EQ(result2.value().schema(), tuple.schema());
    ASSERT_EQ(result2.value().values(), tuple.values());
}
