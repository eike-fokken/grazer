#pragma once
#include <Gasnode.hpp>

namespace Model::Gas {

  /// This class represents an inner node of the gas network, that neither
  /// injects gas nor takes gas out.
  class Innode final : public Gasnode {

  public:
    static std::string get_type();

    // Maybe we should incorporate the min and max pressure later on...
    using Gasnode::Gasnode;

    virtual ~Innode(){};

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;
    void d_evalutate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;

    void d_evalutate_d_last_state(
        Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
        double /*new_time*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const final;
  };

} // namespace Model::Gas
