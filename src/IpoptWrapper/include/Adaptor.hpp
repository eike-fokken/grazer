#ifndef ADAPTOR_HPP
#define ADAPTOR_HPP

#include "IpIpoptApplication.hpp"
#include "Wrapper.hpp"
#include <Eigen/Dense>
#include <memory>

using VectorXd = Eigen::VectorXd;
using VectorXi = Eigen::VectorXi;

template <
    typename ObjFun, typename ObjGradFun, typename ConstrsFun,
    typename ConstrsJacFun>
class IpoptAdaptor {
  using WrapperType
      = IpoptWrapper<ObjFun, ObjGradFun, ConstrsFun, ConstrsJacFun>;

  Ipopt::SmartPtr<WrapperType> _nlp;
  Ipopt::SmartPtr<Ipopt::IpoptApplication> _app;

public:
  IpoptAdaptor(ObjFun f, ObjGradFun grad, ConstrsFun g, ConstrsJacFun jac) :
      _nlp(new WrapperType(f, grad, g, jac)), _app(IpoptApplicationFactory()) {}
  ~IpoptAdaptor() = default;

  void set_initial_point(VectorXd &x0) { _nlp->set_initial_point(x0); }

  void set_variable_bounds(VectorXd &lb, VectorXd &ub) {
    _nlp->set_variable_bounds(lb, ub);
  }

  void set_constraint_bounds(VectorXd &g_lb, VectorXd &g_ub) {
    _nlp->set_constraint_bounds(g_lb, g_ub);
  }

  auto optimize() const {
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

  VectorXd get_solution() const { return _nlp->get_solution(); }
  double get_obj_value() const { return _nlp->get_obj_value(); }
};

#endif /* ADAPTOR_HPP */