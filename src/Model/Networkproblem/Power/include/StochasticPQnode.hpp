#include "Powernode.hpp"

namespace Model::Networkproblem::Power {

  class StochasticPQnode final : public Powernode {
  public:
    static std::string get_type();

    using Powernode::Powernode;

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, const Eigen::Ref<const Eigen::VectorXd> &last_state,
        const Eigen::Ref<const Eigen::VectorXd> &new_state) const override;

    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        const Eigen::Ref<const Eigen::VectorXd> &,
        const Eigen::Ref<const Eigen::VectorXd> &new_state) const override;

    void save_values(
        double time, const Eigen::Ref<const Eigen::VectorXd> &state) override;
  };
} // namespace Model::Networkproblem::Power
