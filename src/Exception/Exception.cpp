#include <Exception.hpp>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>

// void exception_builder(std::vector<std::string> stringvector, char const *
// file, int line) {
//   std::string rawmsg = std::accumulate(stringvector.begin(),
//   stringvector.end(),
//                                        std::string(""));
//   throw Exception(rawmsg, file, line);
// }

Exception::Exception(std::string const &arg, char const *file, int line)
    : std::runtime_error(arg), rawmsg(arg) {
  std::ostringstream o;
  o << file << ":" << line << ": " << arg;
  msg = o.str();
}

char const *Exception::what() const noexcept { return msg.c_str(); }

void exception_builder(std::vector<std::string> stringvector, char const *file,
                       int line) {
  std::string rawmsg = std::accumulate(stringvector.begin(), stringvector.end(),
                                       std::string(""));
  throw Exception(rawmsg, file, line);
}
