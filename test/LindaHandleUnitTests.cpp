#include "LindaCommand.hpp"
#include "LindaHandle.hpp"
#include "LindaTuple.hpp"

#include "gtest/gtest.h"
#include <optional>
#include <thread>

TEST(LINDA_HANDLE_UNIT_TESTS, Handle_read) {
    // given
    int in_fds[2];
    pipe(in_fds);
    int out_fds[2];
    pipe(out_fds);
    linda::TuplePattern pattern = linda::TuplePattern::Builder().anyInt().anyFloat().anyString().build();
    linda::Handle       handle(in_fds[0], out_fds[1]);

    // when
    std::optional<linda::Tuple> result;
    std::thread                 thread([&] { result = handle.read(pattern, 123); });
    linda::Request              request        = linda::Request::receive(out_fds[0]);
    linda::Tuple                response_value = linda::Tuple::Builder().Int(1).Float(2.1).String("test").build();
    linda::Response             response       = linda::Response::Result(response_value);
    response.send(in_fds[1]);
    thread.join();

    // then
    ASSERT_EQ(request.getType(), linda::RequestType::Read);
    ASSERT_EQ(request.getTimeout(), 123);
    ASSERT_EQ(request.getTuplePattern().schema(), pattern.schema());
    ASSERT_EQ(request.getTuplePattern().serialize(), pattern.serialize());
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value().schema(), response_value.schema());
    ASSERT_EQ(result.value().values(), response_value.values());
    ASSERT_EQ(result.value().serialize(), response_value.serialize());
    close(in_fds[0]);
    close(in_fds[1]);
    close(out_fds[0]);
    close(out_fds[1]);
}

TEST(LINDA_HANDLE_UNIT_TESTS, Handle_in) {
    // given
    int in_fds[2];
    pipe(in_fds);
    int out_fds[2];
    pipe(out_fds);
    linda::TuplePattern pattern = linda::TuplePattern::Builder().anyInt().anyFloat().anyString().build();
    linda::Handle       handle(in_fds[0], out_fds[1]);

    // when
    std::optional<linda::Tuple> result;
    std::thread                 thread([&] { result = handle.in(pattern, 123); });
    linda::Request              request        = linda::Request::receive(out_fds[0]);
    linda::Tuple                response_value = linda::Tuple::Builder().Int(1).Float(2.1).String("test").build();
    linda::Response             response       = linda::Response::Result(response_value);
    response.send(in_fds[1]);
    thread.join();

    // then
    ASSERT_EQ(request.getType(), linda::RequestType::In);
    ASSERT_EQ(request.getTimeout(), 123);
    ASSERT_EQ(request.getTuplePattern().schema(), pattern.schema());
    ASSERT_EQ(request.getTuplePattern().serialize(), pattern.serialize());
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value().schema(), response_value.schema());
    ASSERT_EQ(result.value().values(), response_value.values());
    ASSERT_EQ(result.value().serialize(), response_value.serialize());
    close(in_fds[0]);
    close(in_fds[1]);
    close(out_fds[0]);
    close(out_fds[1]);
}

TEST(LINDA_HANDLE_UNIT_TESTS, Handle_out) {
    // given
    int in_fds[2];
    pipe(in_fds);
    int out_fds[2];
    pipe(out_fds);
    linda::Tuple  tuple = linda::Tuple::Builder().Int(1).Float(2.3).String("test").build();
    linda::Handle handle(in_fds[0], out_fds[1]);

    // when
    std::thread    thread([&] { handle.out(tuple); });
    linda::Request request = linda::Request::receive(out_fds[0]);
    thread.join();

    // then
    ASSERT_EQ(request.getType(), linda::RequestType::Out);
    ASSERT_EQ(request.getTimeout(), 0);
    ASSERT_EQ(request.getTuple().schema(), tuple.schema());
    ASSERT_EQ(request.getTuple().values(), tuple.values());
    ASSERT_EQ(request.getTuple().serialize(), tuple.serialize());
    close(in_fds[0]);
    close(in_fds[1]);
    close(out_fds[0]);
    close(out_fds[1]);
}
