#pragma once
#include <stdexcept>
#include <string>
#include <vector>

[[noreturn]] void exception_builder(std::vector<std::string> stringvector,
                                    char const *file, int line);

class Exception : public std::runtime_error {
  std::string msg;

public:
  std::string rawmsg;

  Exception(std::string const &_rawmsg, char const *file, int line);

  ~Exception() noexcept {};
  char const *what() const noexcept override;
};
// #define gthrow(a) exception_builder(a, __FILE__, __LINE__);

#define gthrow(...) exception_builder(__VA_ARGS__, __FILE__, __LINE__);
