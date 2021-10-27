#pragma once
#include "Exception.hpp"
#include <sstream>
#include <type_traits>

namespace Aux {
  /// A version of std::to_string with more precision
  std::string to_string_precise(double value, int n = 9);

  template <class T, class U> constexpr T narrow_cast(U &&u) noexcept {
    return static_cast<T>(std::forward<U>(u));
  }

  // https://github.com/microsoft/GSL/blob/main/include/gsl/narrow#L28
  template <
      class T, class U,
      typename std::enable_if_t<std::is_arithmetic_v<T>> * = nullptr>
  constexpr T narrow(U u) noexcept(false) {

    constexpr const bool is_different_signedness
        = (std::is_signed_v<T> != std::is_signed_v<U>);

    const T t = narrow_cast<T>(u);

    if (static_cast<U>(t) != u
        || (is_different_signedness && ((t < T{}) != (u < U{})))) {
      ghtrow(
          "Narrowing error. This usually happens if a number is too great to "
          "fit into an index type.");
    }
    return t;
  }

} // namespace Aux
