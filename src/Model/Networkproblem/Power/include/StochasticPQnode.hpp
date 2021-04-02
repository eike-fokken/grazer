#pragma once
#include "Normaldistribution.hpp"
#include "Powernode.hpp"
#include <memory>

namespace Model::Networkproblem::Power {

  class StochasticPQnode final : public Powernode {
  public:
    static std::string get_type();

    using Powernode::Powernode;

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state) const override;

    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const>,
        Eigen::Ref<Eigen::VectorXd const> new_state) const override;

    void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state) override;

    void
    save_values(double time, Eigen::Ref<Eigen::VectorXd const> state) override;

  private:
    std::unique_ptr<Aux::Normaldistribution> distribution{
        std::make_unique<Aux::Normaldistribution>()};

    double sigma_P;
    double theta_P;

    double sigma_Q;
    double theta_Q;

    double current_P;
    double current_Q;
  };
} // namespace Model::Networkproblem::Power
