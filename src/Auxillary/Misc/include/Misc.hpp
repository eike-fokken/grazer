#pragma once
#include "Exception.hpp"
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

  // https://github.com/microsoft/GSL/blob/main/include/gsl/narrow#L28
  template <
      class T, class U,
      typename std::enable_if_t<std::is_arithmetic_v<T>> * = nullptr>
  constexpr T narrow(U u) noexcept(false) {

    constexpr const bool is_different_signedness
        = (std::is_signed_v<T> != std::is_signed_v<U>);

    const T t = static_cast<T>(u);

    if (static_cast<U>(t) != u
        || (is_different_signedness && ((t < T{}) != (u < U{})))) {
      gthrow(
          {"Narrowing error. This usually happens if a number is too great to "
           "fit into an index type."});
    }
    return t;
  }

  template <typename T> T safe_addition(T a, T b) {
    if ((a > 0 and a > std::numeric_limits<T>::max() - b)
        or (b < 0 and a < std::numeric_limits<T>::min() - b)) {
      gthrow({"The addition would cause an overflow."});
    }
    return a + b;
  }

} // namespace Aux
