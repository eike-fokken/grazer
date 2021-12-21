#include "Adaptor.hpp"
#include "InterpolatingVector.hpp"
#include "OptimizableObject.hpp"

namespace Optimization {

  IpoptAdaptor::IpoptAdaptor(
      Model::Timeevolver &evolver, Model::OptimizableObject &problem,
      std::vector<double> simulation_timepoints, Eigen::VectorXd initial_state,
      Aux::InterpolatingVector initial_controls,
      Aux::InterpolatingVector lower_bounds,
      Aux::InterpolatingVector upper_bounds,
      Aux::InterpolatingVector constraints_lower_bounds,
      Aux::InterpolatingVector constraints_upper_bounds) :
      _nlp(new Optimization::IpoptWrapper(
          evolver, problem, std::move(simulation_timepoints),
          std::move(initial_state), std::move(initial_controls),
          std::move(lower_bounds), std::move(upper_bounds),
          std::move(constraints_lower_bounds),
          std::move(constraints_upper_bounds))),
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
  Aux::InterpolatingVector_Base const &IpoptAdaptor::get_solution() const {
    return _nlp->get_solution();
  }
  double IpoptAdaptor::get_obj_value() const {
    return _nlp->get_final_objective_value();
  }
} // namespace Optimization
