#pragma once
#include "Normaldistribution.hpp"
#include "Powernode.hpp"
#include <limits>

static_assert(
    std::numeric_limits<double>::has_signaling_NaN == true,
    "StochasticPQnode uses signaling_NaN. If you don't have that, replace the "
    "initialization of current_P and current_Q by something else.");

namespace Model::Power {

  class StochasticPQnode final : public Powernode {
  public:
    static std::string get_type();
    std::string get_power_type() const override;

    static nlohmann::json get_schema();
    static nlohmann::json get_boundary_schema();

    StochasticPQnode(nlohmann::json const &topology);

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;

    void d_evalutate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;

    void d_evalutate_d_last_state(
        Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
        double /*new_time*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const override;

    void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) override;

    void json_save(
        double time, Eigen::Ref<Eigen::VectorXd const> const &state) override;

    // For testing purposes:
    double get_current_P() const;
    double get_current_Q() const;

  private:
    struct StochasticData {
      StochasticData(
          double _stability_parameter, double _cut_off_factor, double _sigma_P,
          double _theta_P, double _sigma_Q, double _theta_Q,
          int _number_of_stochastic_steps,
          std::array<uint32_t, Aux::pcg_seed_count> &used_seed) :
          distribution(used_seed),
          stability_parameter(_stability_parameter),
          cut_off_factor(_cut_off_factor),
          sigma_P(_sigma_P),
          theta_P(_theta_P),
          sigma_Q(_sigma_Q),
          theta_Q(_theta_Q),
          number_of_stochastic_steps(_number_of_stochastic_steps) {}

      StochasticData(
          double _stability_parameter, double _cut_off_factor, double _sigma_P,
          double _theta_P, double _sigma_Q, double _theta_Q,
          int _number_of_stochastic_steps,
          std::array<uint32_t, Aux::pcg_seed_count> &used_seed,
          std::array<uint32_t, Aux::pcg_seed_count> seed) :
          distribution(used_seed, seed),
          stability_parameter(_stability_parameter),
          cut_off_factor(_cut_off_factor),
          sigma_P(_sigma_P),
          theta_P(_theta_P),
          sigma_Q(_sigma_Q),
          theta_Q(_theta_Q),
          number_of_stochastic_steps(_number_of_stochastic_steps) {}

      Aux::Normaldistribution distribution;
      double const stability_parameter;
      double const cut_off_factor;
      double const sigma_P;
      double const theta_P;
      double const sigma_Q;
      double const theta_Q;

      int const number_of_stochastic_steps;
    };
    double current_P{std::numeric_limits<double>::signaling_NaN()};
    double current_Q{std::numeric_limits<double>::signaling_NaN()};
    std::unique_ptr<StochasticData> stochasticdata;
  };
} // namespace Model::Power
