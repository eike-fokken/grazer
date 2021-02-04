#include <Misc.hpp>


namespace Aux {
std::string to_string_precise (double value, int n)
{
  std::ostringstream str;
  str.precision(n);
  str << std::fixed << value;
  return str.str();
}

}
