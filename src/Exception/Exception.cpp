#include <Exception.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

Exception::Exception(const std::string &arg, const char *file, int line)
    : std::runtime_error(arg), rawmsg(arg) {
  std::ostringstream o;
  o << file << ":" << line << ": " << arg;
  msg = o.str();
}
const char *Exception::what() const noexcept { return msg.c_str(); }
