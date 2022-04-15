#include "LindaCommand.hpp"

#include "Buffer.hpp"
#include "LindaTuple.hpp"

#include <vector>

namespace linda
{

Request::Request(RequestType type, int timeout_ms, std::vector<char> &&payload) :
    type_(type), timeout_ms_(timeout_ms), payload_(payload) {}

Request Request::Read(TuplePattern &pattern, int timeout_ms) {
    return Request(RequestType::Read, timeout_ms, pattern.serialize());
}

Request Request::In(TuplePattern &pattern, int timeout_ms) {
    return Request(RequestType::In, timeout_ms, pattern.serialize());
}

Request Request::Out(Tuple &tuple) {
    return Request(RequestType::Out, 0, tuple.serialize());
}

Request Request::Close() {
    return Request(RequestType::Close, 0, std::vector<char>());
}

void Request::send(int fd) {
    BufferEncoder encoder;
    encoder.pushChar(static_cast<char>(type_));
    encoder.pushInt(timeout_ms_);
    encoder.pushData(payload_);
    encoder.send(fd);
}

Request Request::receive(int fd) {
    BufferDecoder     decoder    = BufferDecoder::recv(fd);
    RequestType       type       = static_cast<RequestType>(decoder.readChar());
    int               timeout_ms = decoder.readInt();
    std::vector<char> payload    = decoder.readData();
    return Request(type, timeout_ms, std::move(payload));
}

RequestType Request::getType() {
    return type_;
}

int Request::getTimeout() {
    return timeout_ms_;
}

TuplePattern Request::getTuplePattern() {
    return TuplePattern::deserialize(payload_);
}

Tuple Request::getTuple() {
    return Tuple::deserialize(payload_);
}

Response::Response(ResponseType type) : type_(type) {}

Response::Response(ResponseType type, std::vector<char> &&payload) : type_(type), payload_(payload) {}

Response Response::Timeout() {
    return Response(ResponseType::Timeout);
}

Response Response::Result(Tuple &tuple) {
    return Response(ResponseType::Result, tuple.serialize());
}

void Response::send(int fd) {
    BufferEncoder encoder;
    encoder.pushChar(static_cast<char>(type_));
    encoder.pushData(payload_);
    encoder.send(fd);
}

Response Response::receive(int fd) {
    BufferDecoder     decoder = BufferDecoder::recv(fd);
    ResponseType      type    = static_cast<ResponseType>(decoder.readChar());
    std::vector<char> payload = decoder.readData();
    return Response(type, std::move(payload));
}

ResponseType Response::getType() {
    return type_;
}

Tuple Response::getTuple() {
    return Tuple::deserialize(payload_);
}

} // namespace linda