#include "Adaptor.hpp"
#include "InterpolatingVector.hpp"
#include "OptimizableObject.hpp"
#include "Optimizer.hpp"
#include <memory>

namespace Optimization {

  IpoptAdaptor::IpoptAdaptor(std::unique_ptr<Optimizer> optimizer) :
      _nlp(new Optimization::IpoptWrapper(std::move(optimizer))),
      _app(IpoptApplicationFactory()) {}

  Ipopt::ApplicationReturnStatus IpoptAdaptor::optimize() const {
    // Approximate the hessian
    _app->Options()->SetStringValue("hessian_approximation", "limited-memory");
    // disable Ipopt's console output
    _app->Options()->SetIntegerValue("print_level", 0);
    // Supress Ipopt Banner
    _app->Options()->SetBoolValue("sb", true);
    // Process the options
    auto status = _app->Initialize();
    if (status != Ipopt::Solve_Succeeded) {
      std::cout << "Couldn't find an optimum!" << std::endl;
      return status;
    }
    // solve the problem
    status = _app->OptimizeTNLP(_nlp);
    return status;
  }
  Eigen::VectorXd IpoptAdaptor::get_solution() const {
    return _nlp->get_best_solution();
  }
  double IpoptAdaptor::get_obj_value() const {
    return _nlp->get_best_objective_value();
  }
} // namespace Optimization
