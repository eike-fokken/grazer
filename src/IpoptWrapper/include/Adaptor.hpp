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
        Model::Timedata simulation_data, Model::Timedata controls_data,
        Model::Timedata constraints_data,
        Eigen::Ref<Eigen::VectorXd const> const &initial_state,
        Aux::InterpolatingVector const &initial_controls,
        Aux::InterpolatingVector const &lower_bounds,
        Aux::InterpolatingVector const &upper_bounds,
        Aux::InterpolatingVector const &constraints_lower_bounds,
        Aux::InterpolatingVector const &constraints_upper_bounds);
    ~IpoptAdaptor() = default;

    auto optimize() const;

    Eigen::VectorXd get_solution() const;
    double get_obj_value() const;
  };
} // namespace Optimization
