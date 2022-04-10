#include "../include/LindaCommand.hpp"

#include "LindaTuple.hpp"

#include <cstdio>
#include <unistd.h>
#include <vector>

void write_int(char *ptr, int value) {
    char *int_val = static_cast<char *>(static_cast<void *>(&value));
    for (int i = 0; i < sizeof(int); ++i) {
        ptr[i] = int_val[i];
    }
}

int read_int(char *ptr) {
    int *int_ptr = static_cast<int *>(static_cast<void *>(ptr));
    return *int_ptr;
}

void write_char(char *ptr, char value) {
    *ptr = value;
}

char read_char(char *ptr) {
    return *ptr;
}

void write_buff(char *ptr, std::vector<char> &buff) {
    for (int i = 0; i < buff.size(); ++i) {
        ptr[i] = buff[i];
    }
}

std::vector<char> read_buff(char *ptr, int size) {
    std::vector<char> vec;
    vec.reserve(size);
    for (int i = 0; i < size; ++i) {
        vec.push_back(ptr[i]);
    }
    return vec;
}

namespace linda
{

Request::Request(RequestType type, int timeout_ms, std::vector<char> &&payload) :
    type_(type), timeout_ms_(timeout_ms), payload_(payload) {}

Request Request::Read(TuplePattern &pattern, int timeout_ms) {
    return Request(RequestType::Read, timeout_ms, pattern.serialize());
}

Request Request::In(TuplePattern &pattern, int timeout_ms) {
    return Request(RequestType::Read, timeout_ms, pattern.serialize());
}

Request Request::Out(Tuple &tuple) {
    return Request(RequestType::Out, 0, tuple.serialize());
}

void Request::send(int fd) {
    int               size = sizeof(char) + sizeof(int) + payload_.size();
    std::vector<char> message(sizeof(int) + size, 0);

    write_int(message.data(), size);
    write_char(message.data() + sizeof(int), static_cast<char>(type_));
    write_int(message.data() + sizeof(int) + 1, timeout_ms_);
    write_buff(message.data() + 2 * sizeof(int) + 1, payload_);

    int sent = 0;
    while (sent < size + sizeof(int)) {
        int res = write(fd, message.data() + sent, size + sizeof(int) - sent);
        if (res <= 0) {
            perror("failed to write request");
            throw res;
        } else {
            sent += res;
        }
    }
}

Request Request::receive(int fd) {
    char request_size[sizeof(int)];
    int  recv = 0;
    while (recv < sizeof(int)) {
        int res = read(fd, request_size + recv, sizeof(int) - recv);
        if (res <= 0) {
            perror("failed to read request size");
            throw res;
        } else {
            recv += res;
        }
    }

    int               size = read_int(request_size);
    std::vector<char> buff(size, 0);
    recv = 0;
    while (recv < size) {
        int res = read(fd, buff.data() + recv, size - recv);
        if (res <= 0) {
            perror("failed to read request data");
            throw res;
        } else {
            recv += res;
        }
    }

    RequestType       type       = static_cast<RequestType>(read_char(buff.data()));
    int               timeout_ms = read_int(buff.data() + 1);
    std::vector<char> payload    = read_buff(buff.data() + sizeof(int) + 1, size - sizeof(int) - 1);

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
    int               size = sizeof(char) + payload_.size();
    std::vector<char> message(sizeof(int) + size, 0);

    write_int(message.data(), size);
    write_char(message.data() + sizeof(int), static_cast<char>(type_));
    write_buff(message.data() + sizeof(int) + 1, payload_);

    int sent = 0;
    while (sent < size + sizeof(int)) {
        int res = write(fd, message.data() + sent, size + sizeof(int) - sent);
        if (res <= 0) {
            perror("failed to write response");
            throw res;
        } else {
            sent += res;
        }
    }
}

Response Response::receive(int fd) {
    char request_size[sizeof(int)];
    int  recv = 0;
    while (recv < sizeof(int)) {
        int res = read(fd, request_size + recv, sizeof(int) - recv);
        if (res <= 0) {
            perror("failed to read response size");
            throw res;
        } else {
            recv += res;
        }
    }

    int               size = read_int(request_size);
    std::vector<char> buff(size, 0);
    recv = 0;
    while (recv < size) {
        int res = read(fd, buff.data() + recv, size - recv);
        if (res <= 0) {
            perror("failed to read response data");
            throw res;
        } else {
            recv += res;
        }
    }

    ResponseType      type    = static_cast<ResponseType>(read_char(buff.data()));
    std::vector<char> payload = read_buff(buff.data() + 1, size - 1);

    return Response(type, std::move(payload));
}

ResponseType Response::getType() {
    return type_;
}

Tuple Response::getTuple() {
    return Tuple::deserialize(payload_);
}

} // namespace linda