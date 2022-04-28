#pragma once

#include <../include/LindaTuple.hpp>
#include <vector>

namespace linda
{

enum class RequestType : char
{
    Read  = 'r',
    In    = 'i',
    Out   = 'o',
    Close = 'c'
};

enum class ResponseType : char
{
    Timeout = 'r',
    Result  = 't',
    Done    = 'd',
};

class Request
{
  public:
    static Request Read(TuplePattern &pattern, int timeout_ms);
    static Request In(TuplePattern &pattern, int timeout_ms);
    static Request Out(Tuple &tuple);
    static Request Close();

    void           send(int fd);
    static Request receive(int fd);

    RequestType  getType();
    int          getTimeout();
    TuplePattern getTuplePattern();
    Tuple        getTuple();

  private:
    Request(RequestType type, int timeout_ms, std::vector<char> &&payload);

    RequestType       type_;
    int               timeout_ms_;
    std::vector<char> payload_;
};

class Response
{
  public:
    static Response Timeout();
    static Response Result(Tuple &tuple);
    static Response Done();

    void            send(int fd);
    static Response receive(int fd);

    ResponseType getType();
    Tuple        getTuple();

  private:
    Response(ResponseType type);
    Response(ResponseType type, std::vector<char> &&payload);

    ResponseType      type_;
    std::vector<char> payload_;
};

} // namespace linda