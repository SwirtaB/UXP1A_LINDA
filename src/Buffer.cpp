#include "Buffer.hpp"

#include "Debug.hpp"

#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

void writeIntToChars(char *ptr, int value) {
    char *int_val = static_cast<char *>(static_cast<void *>(&value));
    for (int i = 0; i < sizeof(int); ++i) {
        ptr[i] = int_val[i];
    }
}

int readIntFromChars(char *ptr) {
    int   value;
    char *value_ptr = static_cast<char *>(static_cast<void *>(&value));
    for (int i = 0; i < sizeof(int); ++i) {
        value_ptr[i] = ptr[i];
    }
    return value;
}

float readFloatFromChars(char *ptr) {
    float value;
    char *value_ptr = static_cast<char *>(static_cast<void *>(&value));
    for (int i = 0; i < sizeof(float); ++i) {
        value_ptr[i] = ptr[i];
    }
    return value;
}

void BufferEncoder::pushChar(char value) {
    data_.emplace_back(value);
}

void BufferEncoder::pushInt(int value) {
    char *int_val = static_cast<char *>(static_cast<void *>(&value));
    for (int i = 0; i < sizeof(int); ++i) {
        data_.emplace_back(int_val[i]);
    }
}

void BufferEncoder::pushFloat(float value) {
    char *float_val = static_cast<char *>(static_cast<void *>(&value));
    for (int i = 0; i < sizeof(float); ++i) {
        data_.emplace_back(float_val[i]);
    }
}

void BufferEncoder::pushString(const std::string &value) {
    for (char c : value) {
        data_.emplace_back(c);
    }
    data_.emplace_back(0);
}

void BufferEncoder::pushData(const std::vector<char> &data) {
    pushInt(data.size());
    for (char c : data) {
        data_.emplace_back(c);
    }
}

std::vector<char> BufferEncoder::encode() {
    return data_;
}

void BufferEncoder::send(int fd) {
    std::vector<char> out(data_.size() + sizeof(int));
    writeIntToChars(out.data(), data_.size());
    for (int i = 0; i < data_.size(); ++i) {
        out[i + sizeof(int)] = data_[i];
    }

    int sent = 0;
    while (sent < out.size()) {
        int res = write(fd, out.data() + sent, out.size() - sent);
        if (res <= 0) {
            perror("failed to write request");
            throw std::runtime_error("failed to write request");
        } else {
            sent += res;
        }
    }
}

char BufferDecoder::readChar() {
    char ret = data_[progress_];
    progress_ += 1;
    return ret;
}

int BufferDecoder::readInt() {
    int res = readIntFromChars(data_.data() + progress_);
    progress_ += sizeof(int);
    return res;
}

float BufferDecoder::readFloat() {
    float res = readFloatFromChars(data_.data() + progress_);
    progress_ += sizeof(float);
    return res;
}

std::string BufferDecoder::readString() {
    std::vector<char> str;
    char              c;
    do {
        c = data_[progress_];
        progress_ += 1;
        str.emplace_back(c);
    } while (c != 0);
    return std::string(str.data());
}

std::vector<char> BufferDecoder::readData() {
    int               size = readInt();
    std::vector<char> data;
    data.reserve(size);
    for (int i = 0; i < size; ++i) {
        data.emplace_back(data_[progress_]);
        progress_ += 1;
    }
    return data;
}

BufferDecoder BufferDecoder::decode(const std::vector<char> &data) {
    return BufferDecoder(data);
}

BufferDecoder BufferDecoder::recv(int fd) {
    int   size;
    char *size_ptr = static_cast<char *>(static_cast<void *>(&size));
    int   recv     = 0;
    while (recv < sizeof(int)) {
        int res = read(fd, size_ptr + recv, sizeof(int) - recv);
        if (res <= 0) {
            perror("failed to read request size");
            throw std::runtime_error("failed to read request size");
        } else {
            recv += res;
        }
    }

    BufferDecoder decoder(size);
    recv = 0;
    while (recv < size) {
        int res = read(fd, decoder.data_.data() + recv, size - recv);
        if (res <= 0) {
            perror("failed to read request data");
            throw std::runtime_error("failed to read request data");
        } else {
            recv += res;
        }
    }
    return decoder;
}

BufferDecoder::BufferDecoder(int size) : data_(size, 0), progress_(0) {}

BufferDecoder::BufferDecoder(const std::vector<char> &data) : data_(data), progress_(0) {}
