#include "LindaCommand.hpp"
#include "LindaTuple.hpp"

#include "gtest/gtest.h"
#include <thread>
#include <unistd.h>

TEST(LINDA_COMMAND_UNIT_TESTS, Request_Read) {
    // given
    linda::TuplePattern pattern =
        linda::TuplePattern::Builder().anyInt().floatOf(linda::RequirementType::Eq, 1.23).anyString().build();
    int timeout = 1432;

    // when
    linda::Request request = linda::Request::Read(pattern, timeout);

    // then
    ASSERT_EQ(request.getType(), linda::RequestType::Read);
    ASSERT_EQ(request.getTimeout(), timeout);
    ASSERT_EQ(request.getTuplePattern().schema(), pattern.schema());
}

TEST(LINDA_COMMAND_UNIT_TESTS, Request_In) {
    // given
    linda::TuplePattern pattern =
        linda::TuplePattern::Builder().anyInt().floatOf(linda::RequirementType::Eq, 1.23).anyString().build();
    int timeout = 1432;

    // when
    linda::Request request = linda::Request::In(pattern, timeout);

    // then
    ASSERT_EQ(request.getType(), linda::RequestType::In);
    ASSERT_EQ(request.getTimeout(), timeout);
    ASSERT_EQ(request.getTuplePattern().schema(), pattern.schema());
}

TEST(LINDA_COMMAND_UNIT_TESTS, Request_Out) {
    // given
    linda::Tuple tuple = linda::Tuple::Builder().Int(1).Float(2.34).String("test").build();

    // when
    linda::Request request = linda::Request::Out(tuple);

    // then
    ASSERT_EQ(request.getType(), linda::RequestType::Out);
    ASSERT_EQ(request.getTuple().schema(), tuple.schema());
    ASSERT_EQ(request.getTuple().values(), tuple.values());
}

TEST(LINDA_COMMAND_UNIT_TESTS, Request_Close) {
    // when
    linda::Request request = linda::Request::Close();

    // then
    ASSERT_EQ(request.getType(), linda::RequestType::Close);
}

TEST(LINDA_COMMAND_UNIT_TESTS, Request_send__to__Request_receive) {
    // given
    linda::TuplePattern pattern =
        linda::TuplePattern::Builder().anyInt().anyFloat().stringOf(linda::RequirementType::LessEq, "test").build();
    linda::Request request = linda::Request::In(pattern, 123);

    // when
    int fds[2];
    pipe(fds);
    std::thread    thread([&] { request.send(fds[1]); });
    linda::Request request_received = linda::Request::receive(fds[0]);
    thread.join();

    // then
    ASSERT_EQ(request.getType(), request_received.getType());
    ASSERT_EQ(request.getTimeout(), request_received.getTimeout());
    ASSERT_EQ(request.getTuplePattern().schema(), request_received.getTuplePattern().schema());
    close(fds[0]);
    close(fds[1]);
}

TEST(LINDA_COMMAND_UNIT_TESTS, Response_Timeout) {
    // when
    linda::Response response = linda::Response::Timeout();

    // then
    ASSERT_EQ(response.getType(), linda::ResponseType::Timeout);
}

TEST(LINDA_COMMAND_UNIT_TESTS, Response_Result) {
    // given
    linda::Tuple tuple = linda::Tuple::Builder().Int(1).Float(1.23).String("test").build();

    // when
    linda::Response response = linda::Response::Result(tuple);

    // then
    ASSERT_EQ(response.getType(), linda::ResponseType::Result);
    ASSERT_EQ(response.getTuple().schema(), tuple.schema());
    ASSERT_EQ(response.getTuple().values(), tuple.values());
}

TEST(LINDA_COMMAND_UNIT_TESTS, Response_send__to__Response_receive) {
    // given
    linda::Tuple    tuple    = linda::Tuple::Builder().Int(1).Float(1.23).String("test").build();
    linda::Response response = linda::Response::Result(tuple);

    // when
    int fds[2];
    pipe(fds);
    std::thread     thread([&] { response.send(fds[1]); });
    linda::Response response_received = linda::Response::receive(fds[0]);
    thread.join();

    // then
    ASSERT_EQ(response.getType(), response_received.getType());
    ASSERT_EQ(response.getTuple().schema(), response_received.getTuple().schema());
    ASSERT_EQ(response.getTuple().values(), response_received.getTuple().values());
    close(fds[0]);
    close(fds[1]);
}