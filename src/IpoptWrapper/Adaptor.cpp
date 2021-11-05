#include "Adaptor.hpp"
#include "OptimizableObject.hpp"

namespace Optimization {

  IpoptAdaptor::IpoptAdaptor(
      Model::Timeevolver &evolver, Model::OptimizableObject &problem,
      Model::Timedata simulation_data, Model::Timedata controls_data,
      Model::Timedata constraints_data,
      Eigen::Ref<Eigen::VectorXd const> const &initial_state,
      Aux::InterpolatingVector const &initial_controls,
      Aux::InterpolatingVector const &lower_bounds,
      Aux::InterpolatingVector const &upper_bounds,
      Aux::InterpolatingVector const &constraints_lower_bounds,
      Aux::InterpolatingVector const &constraints_upper_bounds) :
      _nlp(new Optimization::IpoptWrapper(
          evolver, problem, simulation_data, controls_data, constraints_data,
          initial_state, initial_controls, lower_bounds, upper_bounds,
          constraints_lower_bounds, constraints_upper_bounds)),
      _app(IpoptApplicationFactory()) {}
  auto IpoptAdaptor::optimize() const {
    // Approximate the hessian
    _app->Options()->SetStringValue("hessian_approximation", "limited-memory");
    // disable Ipopt's console output
    _app->Options()->SetIntegerValue("print_level", 0);
    // Supress Ipopt Banner
    _app->Options()->SetBoolValue("sb", true);
    // Process the options
    auto status = _app->Initialize();
    // solve the problem
    status = _app->OptimizeTNLP(_nlp);
    return status;
  }
  Eigen::VectorXd IpoptAdaptor::get_solution() const {
    return _nlp->get_solution();
  }
  double IpoptAdaptor::get_obj_value() const {
    return _nlp->get_final_objective_value();
  }
} // namespace Optimization
