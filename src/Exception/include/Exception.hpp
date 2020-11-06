#pragma once
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

class Exception : public std::runtime_error {
  std::string msg;

public:
  std::string rawmsg;

  Exception(std::string const &_rawmsg, char const *file, int line);

  ~Exception() noexcept {};
  char const *what() const noexcept;
};
#define gthrow(rawmsg) throw Exception(rawmsg, __FILE__, __LINE__);
