#include "Normaldistribution.hpp"
#include "Powernode.hpp"
#include <cmath>

static_assert(
    std::numeric_limits<double>::has_infinity,
    "Works only if double has infinities. Possible workaround is to replace "
    "the infinity by a huge value, like 1e15.");

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

    void precompute(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> last_state);

    void
    save_values(double time, Eigen::Ref<Eigen::VectorXd const> state) override;

  private:
    Aux::Normaldistribution distribution;

    double last_sampled_time{-std::numeric_limits<double>::infinity()};

    double sigma_P;
    double theta_P;

    double sigma_Q;
    double theta_Q;

    double current_P;
    double current_Q;
  };
} // namespace Model::Networkproblem::Power
