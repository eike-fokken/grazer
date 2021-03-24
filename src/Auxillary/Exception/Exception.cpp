#include <Exception.hpp>
#include <numeric>
#include <sstream>
#include <stdexcept>

void exception_builder(
    std::vector<std::string> stringvector, char const *file, int line) {
  std::string rawmsg = std::accumulate(
      stringvector.begin(), stringvector.end(), std::string(""));
  std::ostringstream o;
  o << file << ":" << line << ": " << rawmsg;
  std::string msg = o.str();
  throw std::runtime_error(msg);
}
