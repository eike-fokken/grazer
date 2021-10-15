#pragma once
#include "Timedata.hpp"
#include <Eigen/Sparse>
#include <vector>

namespace Aux {
  class Controller {
  public:
    Controller(
        Model::Timedata timedata, Eigen::Index number_of_controls_per_timepoint,
        Eigen::Index number_of_timesteps);

    void set_controls(Eigen::Ref<Eigen::VectorXd> values);

    Eigen::Index get_number_of_controls() const;

    Eigen::Ref<Eigen::VectorXd const> const operator()(double time) const;

    Eigen::Ref<Eigen::VectorXd const> const get_allcontrols() const;

    Eigen::Ref<Eigen::VectorXd> mut_timestep(int time);

  private:
    std::vector<double> const times;
    Eigen::VectorXd allcontrols;
    Eigen::Index const number_of_controls_per_timepoint;
  };
} // namespace Aux
