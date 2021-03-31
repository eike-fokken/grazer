#pragma once
#include <sstream>

namespace Aux {
  /// A version of std::to_string with more precision
  std::string to_string_precise(double value, int n = 9);

} // namespace Aux
