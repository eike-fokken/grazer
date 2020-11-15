#include <Exception.hpp>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>

void gthrow(std::vector<std::string> stringvector) {
  std::string rawmsg = std::accumulate(stringvector.begin(), stringvector.end(),
                                       std::string(""));
  throw Exception(rawmsg, __FILE__, __LINE__);
}

Exception::Exception(std::string const &arg, char const *file, int line)
    : std::runtime_error(arg), rawmsg(arg) {
  std::ostringstream o;
  o << file << ":" << line << ": " << arg;
  msg = o.str();
}
char const *Exception::what() const noexcept { return msg.c_str(); }
