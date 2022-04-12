#include <string>
#include <vector>

class BufferEncoder
{
  public:
    BufferEncoder();

    void pushChar(char value);
    void pushInt(int value);
    void pushFloat(float value);
    void pushString(const std::string &value);
    void pushData(const std::vector<char> &);

    std::vector<char> encode();
    void              send(int fd);

  private:
    std::vector<char> data_;

    void increaseSize(int size);
};

class BufferDecoder
{
  public:
    char              readChar();
    int               readInt();
    float             readFloat();
    std::string       readString();
    std::vector<char> readData();

    static BufferDecoder decode(const std::vector<char> &);
    static BufferDecoder recv(int fd);

  private:
    std::vector<char> data_;
    int               progress_;

    BufferDecoder(int size);
    BufferDecoder(const char *ptr, int size);
};