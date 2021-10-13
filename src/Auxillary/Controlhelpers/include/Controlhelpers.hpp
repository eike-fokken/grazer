#pragma once
#include <Eigen/Sparse>

namespace Aux {
  class Controller {
  public:
    Controller(int number_of_controls_per_timepoint, int number_of_timesteps);

    void set_controls(Eigen::Ref<Eigen::VectorXd> values);

    Eigen::Index get_number_of_controls() const;

    Eigen::Ref<Eigen::VectorXd const> const
    operator()(int current_timestep) const;

    Eigen::Ref<Eigen::VectorXd const> const get_allcontrols() const;

    Eigen::Ref<Eigen::VectorXd> mut_timestep(int current_timestep);

  private:
    Eigen::VectorXd allcontrols;
    int const number_of_controls_per_timepoint;
  };
} // namespace Aux
