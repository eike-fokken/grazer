#pragma once
#include <Eigen/Sparse>

namespace Aux {
  class Controller {
  public:
    Controller(int number_of_controls_per_timepoint, int number_of_timesteps);

    void set_controls(Eigen::Ref<Eigen::VectorXd> values);

    Eigen::Index get_number_of_controls() const;

    Eigen::Ref<Eigen::VectorXd> operator()(int current_timestep) const;

  private:
    Eigen::VectorXd allcontrols;
    int const number_of_controls_per_timepoint;
  };
} // namespace Aux
