#pragma once

#include "Wrapper.hpp"

#include <Eigen/Dense>
#include <IpIpoptApplication.hpp>
#include <memory>

namespace Optimization {
  class Optimizer;

  class IpoptAdaptor {
  private:
    Ipopt::SmartPtr<Optimization::IpoptWrapper> _nlp;
    Ipopt::SmartPtr<Ipopt::IpoptApplication> _app;

  public:
    IpoptAdaptor(std::unique_ptr<Optimizer> optimizer);
    ~IpoptAdaptor() = default;

    Ipopt::ApplicationReturnStatus optimize() const;

    Eigen::VectorXd get_solution() const;
    double get_obj_value() const;
  };
} // namespace Optimization
