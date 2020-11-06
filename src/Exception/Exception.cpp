#include <Exception.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

Exception::Exception(std::string const &arg, char const  *file, int line)
    : std::runtime_error(arg), rawmsg(arg) {
  std::ostringstream o;
  o << file << ":" << line << ": " << arg;
  msg = o.str();
}
char const  *Exception::what() const noexcept { return msg.c_str(); }
