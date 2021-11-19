#pragma once
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
        std::vector<double> simulation_timepoints,
        Eigen::VectorXd initial_state,
        Aux::InterpolatingVector_Base initial_controls,
        Aux::InterpolatingVector_Base lower_bounds,
        Aux::InterpolatingVector_Base upper_bounds,
        Aux::InterpolatingVector_Base constraints_lower_bounds,
        Aux::InterpolatingVector_Base constraints_upper_bounds);
    ~IpoptAdaptor() = default;

    auto optimize() const;

    Eigen::VectorXd const &get_solution() const;
    double get_obj_value() const;
  };
} // namespace Optimization
