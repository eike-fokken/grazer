/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
#pragma once
#include "Exception.hpp"
#include <Eigen/Dense>
#include <sstream>
namespace Aux {
  /// A version of std::to_string with more precision
  std::string to_string_precise(double value, int n = 9);

} // namespace Aux

// Eigen::helpers:

template <typename T>
inline T back(Eigen::Ref<Eigen::VectorX<T> const> const &vector) {
  if (vector.size() == 0) {
    gthrow({"You cannot call 'back' on an empty vector!"});
  }
  return vector[vector.size() - 1];
}

template <typename T> inline T back(Eigen::VectorX<T> const &vector) {
  if (vector.size() == 0) {
    gthrow({"You cannot call 'back' on an empty vector!"});
  }
  return vector[vector.size() - 1];
}

template <typename T> inline T &back(Eigen::Ref<Eigen::VectorX<T>> &vector) {
  if (vector.size() == 0) {
    gthrow({"You cannot call 'back' on an empty vector!"});
  }
  return vector[vector.size() - 1];
}

template <typename T> inline T &back(Eigen::VectorX<T> &vector) {
  if (vector.size() == 0) {
    gthrow({"You cannot call 'back' on an empty vector!"});
  }
  return vector[vector.size() - 1];
}

template <typename T>
inline Eigen::Index
back_index(Eigen::Ref<Eigen::VectorX<T> const> const &vector) {
  if (vector.size() == 0) {
    gthrow({"You cannot call 'back_index' on an empty vector!"});
  }
  return vector.size() - 1;
}
template <typename T>
inline Eigen::Index back_index(Eigen::VectorX<T> const &vector) {
  if (vector.size() == 0) {
    gthrow({"You cannot call 'back_index' on an empty vector!"});
  }
  return vector.size() - 1;
}

template <typename T>
inline T front(Eigen::Ref<Eigen::VectorX<T> const> const &vector) {
  if (vector.size() == 0) {
    gthrow({"You cannot call 'front' on an empty vector!"});
  }
  return vector[0];
}

template <typename T> inline T front(Eigen::VectorX<T> const &vector) {
  if (vector.size() == 0) {
    gthrow({"You cannot call 'front' on an empty vector!"});
  }
  return vector[0];
}

template <typename T> inline T &front(Eigen::Ref<Eigen::VectorX<T>> &vector) {
  if (vector.size() == 0) {
    gthrow({"You cannot call 'front' on an empty vector!"});
  }
  return vector[0];
}

template <typename T> inline T &front(Eigen::VectorX<T> &vector) {
  if (vector.size() == 0) {
    gthrow({"You cannot call 'front' on an empty vector!"});
  }
  return vector[0];
}
