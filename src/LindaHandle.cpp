#include "LindaHandle.hpp"
#include "LindaCommand.hpp"

#include <optional>

namespace linda
{

Handle::Handle(int in, int out) : in_pipe_(in), out_pipe_(out) {}

std::optional<Tuple> Handle::read(TuplePattern &pattern, int timeout_ms) {
    Request::Read(pattern, timeout_ms).send(out_pipe_);
    Response response = Response::receive(in_pipe_);
    if (response.getType() == ResponseType::Timeout) {
        return std::nullopt;
    } else {
        return response.getTuple();
    }
}

std::optional<Tuple> Handle::in(TuplePattern &pattern, int timeout_ms) {
    Request::In(pattern, timeout_ms).send(out_pipe_);
    Response response = Response::receive(in_pipe_);
    if (response.getType() == ResponseType::Timeout) {
        return std::nullopt;
    } else {
        return response.getTuple();
    }
}

void Handle::out(Tuple &tuple) {
    Request::Out(tuple).send(out_pipe_);
}

void Handle::close() {
    Request::Close().send(out_pipe_);
}

} // namespace linda