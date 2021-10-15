#pragma once

#include <Eigen/Sparse>
#include <nlohmann/json.hpp>
namespace Aux {

  class Indexmanager {
  public:
    int get_startindex() const;
    int get_afterindex() const;
    int get_number_of_indices() const;

    int set_state_indices(int next_free_index, int number_of_needed_indices);

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> vector_to_fill,
        nlohmann::json const &initial_json, nlohmann::json const &schema);

  private:
    int startindex{-1};
    int afterindex{-1};
  };

} // namespace Aux
