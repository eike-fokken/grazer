#pragma once
#include <Eigen/Dense>
#include <sstream>

namespace Aux {
  /// A version of std::to_string with more precision
  std::string to_string_precise(double value, int n = 9);

} // namespace Aux

// Eigen::helpers:

template <typename T>
inline T back(Eigen::Ref<Eigen::VectorX<T> const> const &vector) {
  return vector[vector.size() - 1];
}

template <typename T> inline T back(Eigen::VectorX<T> const &vector) {
  return vector[vector.size() - 1];
}

template <typename T> inline T &back(Eigen::Ref<Eigen::VectorX<T>> &vector) {
  return vector[vector.size() - 1];
}

template <typename T> inline T &back(Eigen::VectorX<T> &vector) {
  return vector[vector.size() - 1];
}

template <typename T>
inline Eigen::Index
back_index(Eigen::Ref<Eigen::VectorX<T> const> const &vector) {
  return vector.size() - 1;
}
template <typename T>
inline Eigen::Index back_index(Eigen::VectorX<T> const &vector) {
  return vector.size() - 1;
}

template <typename T>
inline T front(Eigen::Ref<Eigen::VectorX<T> const> const &vector) {
  return vector[0];
}

template <typename T> inline T front(Eigen::VectorX<T> const &vector) {
  return vector[0];
}

template <typename T> inline T &front(Eigen::Ref<Eigen::VectorX<T>> &vector) {
  return vector[0];
}

template <typename T> inline T &front(Eigen::VectorX<T> &vector) {
  return vector[0];
}
