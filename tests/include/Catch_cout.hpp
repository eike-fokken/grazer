#include <iostream>
#include <streambuf>
struct Catch_cout {
  Catch_cout(std::streambuf *new_buffer) : old(std::cout.rdbuf(new_buffer)) {}

  ~Catch_cout() { std::cout.rdbuf(old); }

private:
  std::streambuf *old;
};
