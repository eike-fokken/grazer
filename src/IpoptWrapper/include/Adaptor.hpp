#pragma once
#include "InterpolatingVector.hpp"
#include "Wrapper.hpp"

#include <Eigen/Dense>
#include <IpIpoptApplication.hpp>

namespace Model {
  class OptimizableObject;
}

namespace Optimization {
  class IpoptAdaptor {
  private:
    Ipopt::SmartPtr<Optimization::IpoptWrapper> _nlp;
    Ipopt::SmartPtr<Ipopt::IpoptApplication> _app;

  public:
    IpoptAdaptor(
        Model::Timeevolver &evolver, Model::OptimizableObject &problem,
        Eigen::VectorXd _state_timepoints, Eigen::VectorXd _control_timepoints,
        Eigen::VectorXd _constraint_timepoints, Eigen::VectorXd initial_state,
        Aux::InterpolatingVector initial_controls,
        Aux::InterpolatingVector lower_bounds,
        Aux::InterpolatingVector upper_bounds,
        Aux::InterpolatingVector constraints_lower_bounds,
        Aux::InterpolatingVector constraints_upper_bounds);
    ~IpoptAdaptor() = default;

    auto optimize() const;

    Aux::InterpolatingVector_Base const &get_solution() const;
    double get_obj_value() const;
  };
} // namespace Optimization
