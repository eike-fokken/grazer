#pragma once

#include "Controlhelpers.hpp"
#include "Timedata.hpp"

#include <Eigen/Sparse>
#include <nlohmann/json.hpp>
namespace Aux {

  class Indexmanager {
  protected:
    Indexmanager(){};
    ~Indexmanager();

  public:
    int get_startindex() const;
    int get_afterindex() const;
    int get_number_of_indices() const;
    int set_indices(int next_free_index, int number_of_needed_indices);

  protected:
    int startindex{-1};
    int afterindex{-1};
  };

  class Timeless_Indexmanager final : public Indexmanager {
  public:
    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> vector_to_be_filled,
        nlohmann::json const &initial_json,
        nlohmann::json const &initial_schema);
  };

  class Timed_Indexmanager final : public Indexmanager {
  public:
    void set_initial_values(
        Model::Timedata timedata,
        Aux::Controller &vector_controller_to_be_filled,
        nlohmann::json const &initial_json,
        nlohmann::json const &initial_schema);
  };

} // namespace Aux
