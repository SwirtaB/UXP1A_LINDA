#include "Buffer.hpp"

#include <unistd.h>

BufferEncoder::BufferEncoder() : data_(sizeof(int), 0) {}

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

int readFloatFromChars(char *ptr) {
    float value;
    char *value_ptr = static_cast<char *>(static_cast<void *>(&value));
    for (int i = 0; i < sizeof(float); ++i) {
        value_ptr[i] = ptr[i];
    }
    return value;
}

void BufferEncoder::pushChar(char value) {
    data_.emplace_back(value);
    increaseSize(1);
}

void BufferEncoder::pushInt(int value) {
    char *int_val = static_cast<char *>(static_cast<void *>(&value));
    for (int i = 0; i < sizeof(int); ++i) {
        data_.emplace_back(int_val[i]);
    }
    increaseSize(sizeof(int));
}

void BufferEncoder::pushFloat(float value) {
    char *float_val = static_cast<char *>(static_cast<void *>(&value));
    for (int i = 0; i < sizeof(float); ++i) {
        data_.emplace_back(float_val[i]);
    }
    increaseSize(sizeof(float));
}

void BufferEncoder::pushString(const std::string &value) {
    for (char c : value) {
        data_.emplace_back(c);
    }
    data_.emplace_back(0);
    increaseSize(value.size() + 1);
}

void BufferEncoder::pushData(const std::vector<char> &data) {
    pushInt(data.size());
    for (char c : data) {
        data_.emplace_back(c);
    }
    increaseSize(sizeof(int) + data.size());
}

std::vector<char> BufferEncoder::encode() {
    return std::move(data_);
}

void BufferEncoder::send(int fd) {
    int sent = 0;
    while (sent < data_.size()) {
        int res = write(fd, data_.data() + sent, data_.size() - sent);
        if (res <= 0) {
            perror("failed to write request");
            throw res;
        } else {
            sent += res;
        }
    }
}

void BufferEncoder::increaseSize(int size) {
    int current = readIntFromChars(data_.data());
    writeIntToChars(data_.data(), current + size);
}

char BufferDecoder::readChar() {
    return data_[progress_++];
}

int BufferDecoder::readInt() {
    int res = readIntFromChars(data_.data() + progress_);
    progress_ += sizeof(int);
    return res;
}

float BufferDecoder::readFloat() {
    int res = readFloatFromChars(data_.data() + progress_);
    progress_ += sizeof(float);
    return res;
}

std::string BufferDecoder::readString() {
    std::vector<char> str;
    char              c;
    do {
        c = data_[progress_++];
        str.emplace_back(c);
    } while (c != 0);
    progress_ += str.size();
    return std::string(str.data());
}

std::vector<char> BufferDecoder::readData() {
    int               size = readInt();
    std::vector<char> data;
    data.reserve(size);
    for (int i = 0; i < size; ++i) {
        data.emplace_back(data_[progress_++]);
    }
    return data;
}

BufferDecoder BufferDecoder::decode(const std::vector<char> &data) {
    return BufferDecoder(data.data() + sizeof(int), data.size() - sizeof(int));
}

BufferDecoder BufferDecoder::recv(int fd) {
    int   size;
    char *size_ptr = static_cast<char *>(static_cast<void *>(&size_ptr));
    int   recv     = 0;
    while (recv < sizeof(int)) {
        int res = read(fd, size_ptr + recv, sizeof(int) - recv);
        if (res <= 0) {
            perror("failed to read request size");
            throw res;
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
            throw res;
        } else {
            recv += res;
        }
    }
    return decoder;
}

BufferDecoder::BufferDecoder(int size) : data_(size, 0), progress_(0) {}

BufferDecoder::BufferDecoder(const char *ptr, int size) : data_(ptr, ptr + size), progress_(0) {}
