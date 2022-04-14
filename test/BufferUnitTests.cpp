#include "Buffer.hpp"

#include "gtest/gtest.h"
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>

TEST(BUFFER_UNIT_TESTS, BufferEncoder_pushChar) {
    // given
    BufferEncoder     encoder;
    std::vector<char> test_case;

    // when
    srand(time(NULL));
    char value = rand();

    encoder.pushChar(value);
    test_case.emplace_back(value);

    // then
    ASSERT_EQ(encoder.encode(), test_case);
}

TEST(BUFFER_UNIT_TESTS, BufferEncoder_pushInt) {
    // given
    BufferEncoder     encoder;
    std::vector<char> test_case;

    // when
    srand(time(NULL));
    int value = rand();

    encoder.pushInt(value);

    char *value_ptr = static_cast<char *>(static_cast<void *>(&value));
    for (int i = 0; i < sizeof(int); ++i) {
        test_case.emplace_back(value_ptr[i]);
    }

    // then
    ASSERT_EQ(encoder.encode(), test_case);
}

TEST(BUFFER_UNIT_TESTS, BufferEncoder_pushFloat) {
    // given
    BufferEncoder     encoder;
    std::vector<char> test_case;

    // when
    srand(time(NULL));
    float value = rand() / 1000.0;

    encoder.pushFloat(value);

    char *value_ptr = static_cast<char *>(static_cast<void *>(&value));
    for (int i = 0; i < sizeof(int); ++i) {
        test_case.emplace_back(value_ptr[i]);
    }

    // then
    ASSERT_EQ(encoder.encode(), test_case);
}

TEST(BUFFER_UNIT_TESTS, BufferEncoder_pushString) {
    // given
    BufferEncoder     encoder;
    std::vector<char> test_case;

    // when
    encoder.pushString("HELLO TEST");
    for (char c : {'H', 'E', 'L', 'L', 'O', ' ', 'T', 'E', 'S', 'T', '\0'}) {
        test_case.emplace_back(c);
    }

    // then
    ASSERT_EQ(encoder.encode(), test_case);
}

TEST(BUFFER_UNIT_TESTS, BufferEncoder_pushData) {
    // given
    BufferEncoder     encoder;
    std::vector<char> test_case({1, 5, 2, 4, 5, 6, 7, 2, 4, 5});

    // when
    encoder.pushData(test_case);

    // then
    std::vector<char> valid;
    int               test_case_size = test_case.size();
    char             *ptr            = static_cast<char *>(static_cast<void *>(&test_case_size));
    for (int i = 0; i < sizeof(int); ++i) {
        valid.emplace_back(ptr[i]);
    }
    for (char c : test_case) {
        valid.emplace_back(c);
    }

    ASSERT_EQ(encoder.encode(), valid);
}

TEST(BUFFER_UNIT_TESTS, BufferEncoder_send) {
    // given
    BufferEncoder encoder;
    int           fds[2];
    pipe(fds);

    // when
    srand(time(NULL));
    int value = rand();
    encoder.pushInt(value);
    encoder.send(fds[1]);
    close(fds[1]);

    // then
    std::vector<char> buffer(100, '\0');
    while (read(fds[0], buffer.data(), 100) > 0) {}

    std::vector<char> test_case;
    int               test_case_size = sizeof(int);
    char             *ptr            = static_cast<char *>(static_cast<void *>(&test_case_size));
    for (int i = 0; i < sizeof(int); ++i) {
        test_case.emplace_back(ptr[i]);
    }
    ptr = static_cast<char *>(static_cast<void *>(&value));
    for (int i = 0; i < sizeof(int); ++i) {
        test_case.emplace_back(ptr[i]);
    }
    while (test_case.size() < 100)
        test_case.emplace_back(0);

    ASSERT_EQ(buffer, test_case);
}

TEST(BUFFER_UNIT_TESTS, BufferDecoder_readChar) {
    // given
    BufferEncoder encoder;

    // when
    srand(time(NULL));
    char value = rand();
    encoder.pushChar(value);
    BufferDecoder decoder = BufferDecoder::decode(encoder.encode());

    // then
    ASSERT_EQ(decoder.readChar(), value);
}

TEST(BUFFER_UNIT_TESTS, BufferDecoder_readInt) {
    // given
    BufferEncoder encoder;

    // when
    srand(time(NULL));
    int value = rand();
    encoder.pushInt(value);
    BufferDecoder decoder = BufferDecoder::decode(encoder.encode());

    // then
    ASSERT_EQ(decoder.readInt(), value);
}

TEST(BUFFER_UNIT_TESTS, BufferDecoder_readFloat) {
    // given
    BufferEncoder encoder;

    // when
    srand(time(NULL));
    float value = rand() / 1000.0;
    encoder.pushFloat(value);
    BufferDecoder decoder = BufferDecoder::decode(encoder.encode());

    // then
    ASSERT_EQ(decoder.readFloat(), value);
}

TEST(BUFFER_UNIT_TESTS, BufferDecoder_readString) {
    // given
    BufferEncoder encoder;

    // when
    encoder.pushString("hello test");
    BufferDecoder decoder = BufferDecoder::decode(encoder.encode());

    // then
    ASSERT_EQ(decoder.readString(), std::string("hello test"));
}

TEST(BUFFER_UNIT_TESTS, BufferDecoder_readData) {
    // given
    BufferEncoder encoder;

    // when
    std::vector<char> data({1, 5, 23, 52, 123, -13});
    encoder.pushData(data);
    BufferDecoder decoder = BufferDecoder::decode(encoder.encode());

    // then
    ASSERT_EQ(decoder.readData(), data);
}

TEST(BUFFER_UNIT_TESTS, BufferEncoder_encode__to__BufferDecoder_decode) {
    // given
    BufferEncoder     encoder;
    char              charEncode   = 63;
    int               intEncode    = 4325;
    float             floatEncode  = -142.34123;
    std::string       stringEncode = "asdfax ad";
    std::vector<char> dataEncode({1, 42, 12, 123, -2});

    // when
    encoder.pushChar(charEncode);
    encoder.pushInt(intEncode);
    encoder.pushFloat(floatEncode);
    encoder.pushString(stringEncode);
    encoder.pushData(dataEncode);
    BufferDecoder decoder = BufferDecoder::decode(encoder.encode());

    // then
    ASSERT_EQ(decoder.readChar(), charEncode);
    ASSERT_EQ(decoder.readInt(), intEncode);
    ASSERT_EQ(decoder.readFloat(), floatEncode);
    ASSERT_EQ(decoder.readString(), stringEncode);
    ASSERT_EQ(decoder.readData(), dataEncode);
}

TEST(BUFFER_UNIT_TESTS, BufferEncoder_send__to__BufferDecoder_recv) {
    // given
    BufferEncoder     encoder;
    char              charEncode   = 63;
    int               intEncode    = 4325;
    float             floatEncode  = -142.34123;
    std::string       stringEncode = "asdfax ad";
    std::vector<char> dataEncode({1, 42, 12, 123, -2});

    // when
    encoder.pushChar(charEncode);
    encoder.pushInt(intEncode);
    encoder.pushFloat(floatEncode);
    encoder.pushString(stringEncode);
    encoder.pushData(dataEncode);
    int fds[2];
    pipe(fds);
    encoder.send(fds[1]);
    BufferDecoder decoder = BufferDecoder::recv(fds[0]);

    // then
    ASSERT_EQ(decoder.readChar(), charEncode);
    ASSERT_EQ(decoder.readInt(), intEncode);
    ASSERT_EQ(decoder.readFloat(), floatEncode);
    ASSERT_EQ(decoder.readString(), stringEncode);
    ASSERT_EQ(decoder.readData(), dataEncode);
}
