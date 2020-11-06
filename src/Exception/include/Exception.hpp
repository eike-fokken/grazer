#pragma once
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

class Exception : public std::runtime_error {
  std::string msg;

public:
  std::string rawmsg;

  Exception(const std::string &_rawmsg, const char *file, int line);

  ~Exception() noexcept {};
  const char *what() const noexcept;
};
#define gthrow(rawmsg) throw Exception(rawmsg, __FILE__, __LINE__);
