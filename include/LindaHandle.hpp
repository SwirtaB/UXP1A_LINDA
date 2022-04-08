#pragma once

namespace linda {

class Handle {
public:
  Handle(int in, int out);

  void in();
  void out();
  void read();

private:
  int in_pipe;
  int out_pipe;
};

} // namespace linda