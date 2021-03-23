#pragma once
#include <Eigen/Dense>
#include <iostream>
#include <nlohmann/json.hpp>
#include <streambuf>

struct Catch_cout {
  Catch_cout(std::streambuf *new_buffer) : old(std::cout.rdbuf(new_buffer)) {}

  ~Catch_cout() { std::cout.rdbuf(old); }

private:
  std::streambuf *old;
};

namespace Eigen {
  template <typename Derived>
  void to_json(nlohmann::json &j, const MatrixBase<Derived> &eig);
}

// this is for T = double or T = Eigen::Vector...
template <typename T>
nlohmann::json make_value_json(std::string id, std::string key, T &condition0,
                               T &condition1);

// this is for T = double or T = Eigen::Vector...
template <typename T>
nlohmann::json make_value_json(std::string id, std::string key,
                               std::vector<std::pair<double, T>> pairs);

nlohmann::json
make_full_json(std::map<std::string, std::vector<nlohmann::json>> nodemap,
               std::map<std::string, std::vector<nlohmann::json>> edgemap);

nlohmann::json
make_initial_json(std::map<std::string, std::vector<nlohmann::json>> nodemap,
                  std::map<std::string, std::vector<nlohmann::json>> edgemap);

template <typename Derived>
void Eigen::to_json(nlohmann::json &j, const MatrixBase<Derived> &eig) {
  EIGEN_STATIC_ASSERT_VECTOR_ONLY(MatrixBase<Derived>);
  // static_assert(eig.cols() == 1,
  //               "This function is only for vectors, not for
  //               matrices.");
  j = nlohmann::json::array();

  for (Eigen::Index i = 0; i != eig.rows(); ++i) {
    j.push_back(eig(i));
  }
}

template <typename T>
nlohmann::json make_value_json(std::string id, std::string key, T &condition0,
                               T &condition1) {
  nlohmann::json bound;
  bound["id"] = id;
  bound["data"] = nlohmann::json::array();
  nlohmann::json b0;
  nlohmann::json b1;
  b1[key] = 1;
  b0[key] = 0;
  if constexpr (std::is_same_v<T, double>) {
    b0["values"].push_back(condition0);
    b1["values"].push_back(condition1);
  } else {
    b0["values"] = condition0;
    b1["values"] = condition1;
  }
  bound["data"].push_back(b0);
  bound["data"].push_back(b1);
  return bound;
}

template <typename T>
nlohmann::json make_value_json(std::string id, std::string key,
                               std::vector<std::pair<double, T>> pairs) {

  if (pairs.empty()) {
    throw std::runtime_error("Pairs empty.");
  }
  nlohmann::json bound;
  bound["id"] = id;
  bound["data"] = nlohmann::json::array();

  for (auto [x, values] : pairs) {
    nlohmann::json b;
    b[key] = x;
    if constexpr (std::is_same_v<T, double>) {
      b["values"].push_back(values);
    } else {
      b["values"] = values;
    }
    bound["data"].push_back(b);
  }
  return bound;
}
