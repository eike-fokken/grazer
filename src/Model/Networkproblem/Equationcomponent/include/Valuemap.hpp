#pragma once
#include "Eigen/Sparse"
#include "Exception.hpp"
#include "Mathfunctions.hpp"
#include <functional>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace Model::Networkproblem {

  /// \brief This exception class returns information on whether too high or too
  /// low a value was requested.
  ///
  /// The returned data also indicates the maximal (or minimal) valid argument.

  class Valuemap_out_of_range : public std::out_of_range {
  public:
    Valuemap_out_of_range(std::string message) : std::out_of_range(message) {}
  };

  template <int N> class Valuemap {

  public:
    Valuemap(nlohmann::json const &value_json, std::string key) :
        values(set_condition(value_json, key)){};

    // Valuemap(std::map<double, Eigen::Matrix<double, N, 1>> _values):
    //   values(_values) {};

    Eigen::Matrix<double, N, 1> operator()(double t) const {

      // Note that by virtue of the function set_condition, this map is never
      // empty!
      // last element of the map:
      auto last_element = values.rbegin();
      auto last_t = values.rbegin()->first;
      // first element of the map:
      auto first_element = values.begin();
      auto first_t = values.begin()->first;

      // This catches problems with doubles
      if (t >= last_t) {
        if (t > last_t + Aux::EPSILON) {
          std::ostringstream error_message;
          error_message << "Requested value " << t
                        << " is higher than the last valid value: " << last_t
                        << "\n";

          throw Valuemap_out_of_range(error_message.str());
        }

        return last_element->second;
      }
      if (t <= first_t) {
        if (t < first_t - Aux::EPSILON) {
          std::ostringstream error_message;
          error_message << "Requested value " << t
                        << " is lower than the first valid value: " << first_t
                        << "\n";
          throw Valuemap_out_of_range(error_message.str());
        }

        return first_element->second;
      }
      auto argument_less
          = [](std::pair<double, Eigen::Matrix<double, N, 1>> const &_pair,
               const double argument) { return _pair.first < argument; };

      auto next
          = std::lower_bound(values.begin(), values.end(), t, argument_less);
      auto previous = std::prev(next);

      double t_minus = previous->first;
      Eigen::VectorXd value_minus = previous->second;
      double t_plus = next->first;
      Eigen::VectorXd value_plus = next->second;

      Eigen::VectorXd value
          = previous->second
            + (t - t_minus) * (value_plus - value_minus) / (t_plus - t_minus);
      return value;
    }

    static std::vector<std::pair<double, Eigen::Matrix<double, N, 1>>>
    set_condition(nlohmann::json values_json, std::string key) {

      std::vector<std::pair<double, Eigen::Matrix<double, N, 1>>> pair_values;

      if (values_json["data"].size() == 0) {
        gthrow({"data in node with id ", values_json["id"], " is empty!"})
      }

      for (auto &datapoint : values_json["data"]) {
        if (datapoint["values"].size() != N) {
          gthrow(
              {"Wrong number of initial/boundary values in node ",
               values_json["id"]});
        }
        Eigen::Matrix<double, N, 1> value;
        try {
          for (unsigned int i = 0; i < N; ++i) {
            // auto ijson= static_cast<nlohmann::basic_json::size_type>(i);
            value[i] = datapoint["values"][i];
          }
        } catch (...) {
          gthrow(
              {"initial/boundary data in node with id ", values_json["id"],
               " couldn't be assignd in vector, not a double?"})
        }
        pair_values.push_back({datapoint[key], value});
      }

      auto argument_less
          = [](std::pair<double, Eigen::Matrix<double, N, 1>> const &pair1,
               std::pair<double, Eigen::Matrix<double, N, 1>> const &pair2) {
              return pair1.first < pair2.first;
            };
      auto compare_eq
          = [](std::pair<double, Eigen::Matrix<double, N, 1>> const &pair1,
               std::pair<double, Eigen::Matrix<double, N, 1>> const &pair2) {
              return pair1.first == pair2.first;
            };

      std::sort(pair_values.begin(), pair_values.end(), argument_less);

      auto first_eq_pair = std::adjacent_find(
          pair_values.begin(), pair_values.end(), compare_eq);

      if (first_eq_pair != pair_values.end()) {
        gthrow(
            {"The value ", std::to_string(first_eq_pair->first),
             " appears twice in node with id ", values_json["id"], "."});
      }

      return pair_values;
    }

  private:
    // nlohmann::json values;

    // std::map<double, Eigen::Matrix<double, N, 1>> const values;
    std::vector<std::pair<double, Eigen::Matrix<double, N, 1>>> const values;
  };

  extern template class Valuemap<1>;
  extern template class Valuemap<2>;
  extern template class Valuemap<4>;

} // namespace Model::Networkproblem
