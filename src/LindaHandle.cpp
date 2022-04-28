#include "LindaHandle.hpp"

#include "LindaCommand.hpp"

#include <optional>
#include <stdexcept>

namespace linda
{

Handle::Handle(int in, int out) : in_pipe_(in), out_pipe_(out) {}

std::optional<Tuple> Handle::read(TuplePattern &pattern, int timeout_ms) {
    Request::Read(pattern, timeout_ms).send(out_pipe_);
    Response response = Response::receive(in_pipe_);
    if (response.getType() == ResponseType::Timeout) {
        return std::nullopt;
    } else if (response.getType() == ResponseType::Result) {
        return response.getTuple();
    } else {
        throw std::runtime_error("Handle::read - got invalid response");
    }
}

std::optional<Tuple> Handle::in(TuplePattern &pattern, int timeout_ms) {
    Request::In(pattern, timeout_ms).send(out_pipe_);
    Response response = Response::receive(in_pipe_);
    if (response.getType() == ResponseType::Timeout) {
        return std::nullopt;
    } else if (response.getType() == ResponseType::Result) {
        return response.getTuple();
    } else {
        throw std::runtime_error("Handle::in - got invalid response");
    }
}

void Handle::out(Tuple &tuple) {
    Request::Out(tuple).send(out_pipe_);
    Response response = Response::receive(in_pipe_);
    if (response.getType() != ResponseType::Done) {
        throw std::runtime_error("Handle::out - got invalid response");
    }
}

void Handle::close() {
    Request::Close().send(out_pipe_);
    Response response = Response::receive(in_pipe_);
    if (response.getType() != ResponseType::Done) {
        throw std::runtime_error("Handle::close - got invalid response");
    }
}

} // namespace linda