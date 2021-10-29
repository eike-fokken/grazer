#pragma once
#include <sstream>

namespace Aux {
  /**
   * @brief A version of std::to_string with more precision
   *
   * @param    value                TODO
   * @param    n                    TODO
   * @return std::string
   */
  std::string to_string_precise(double value, int n = 9);

} // namespace Aux
