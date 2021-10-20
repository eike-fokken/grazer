#include "StochasticPQnode.hpp"
#include "Powernode.hpp"
#include "Stochastic.hpp"
#include "make_schema.hpp"
#include <fstream>

namespace Model::Power {

  std::string StochasticPQnode::get_type() { return "StochasticPQnode"; }
  std::string StochasticPQnode::get_power_type() const { return get_type(); }

  nlohmann::json StochasticPQnode::get_schema() {
    nlohmann::json schema = Powernode::get_schema();
    Aux::schema::add_required(
        schema, "stability_parameter", Aux::schema::type::number());
    Aux::schema::add_required(
        schema, "cut_off_factor", Aux::schema::type::number());

    Aux::schema::add_required(schema, "sigma_P", Aux::schema::type::number());
    Aux::schema::add_required(schema, "theta_P", Aux::schema::type::number());
    Aux::schema::add_required(schema, "sigma_Q", Aux::schema::type::number());
    Aux::schema::add_required(schema, "theta_Q", Aux::schema::type::number());
    Aux::schema::add_required(
        schema, "number_of_stochastic_steps", Aux::schema::type::number());
    return schema;
  }

  std::optional<nlohmann::json> StochasticPQnode::get_boundary_schema() {
    auto boundary_schema = Aux::schema::make_boundary_schema(2);
    auto seed_schema
        = Aux::schema::make_list_schema_of(Aux::schema::type::number());
    Aux::schema::add_property(boundary_schema, "seed", seed_schema);
    return boundary_schema;
  }

  StochasticPQnode::StochasticPQnode(nlohmann::json const &topology) :
      Powernode(topology) {
    std::array<uint32_t, Aux::pcg_seed_count> used_seed;
    if (topology["boundary_values"].contains("seed")) {
      stochasticdata = std::make_unique<StochasticData>(
          topology["stability_parameter"].get<double>(),
          topology["cut_off_factor"].get<double>(),
          topology["sigma_P"].get<double>(), topology["theta_P"].get<double>(),
          topology["sigma_Q"].get<double>(), topology["theta_Q"].get<double>(),
          topology["number_of_stochastic_steps"].get<int>(), used_seed,
          topology["boundary_values"]["seed"]
              .get<std::array<uint32_t, Aux::pcg_seed_count>>());
    } else {
      stochasticdata = std::make_unique<StochasticData>(
          topology["stability_parameter"].get<double>(),
          topology["cut_off_factor"].get<double>(),
          topology["sigma_P"].get<double>(), topology["theta_P"].get<double>(),
          topology["sigma_Q"].get<double>(), topology["theta_Q"].get<double>(),
          topology["number_of_stochastic_steps"].get<int>(), used_seed);
    }
    auto &output = get_output_json_ref();
    output["seed"] = used_seed;
  }

  void StochasticPQnode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double /*last_time*/,
      double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    auto V_index = get_state_startindex();
    auto phi_index = V_index + 1;
    rootvalues[V_index] = P(new_state) - current_P;
    rootvalues[phi_index] = Q(new_state) - current_Q;
  }

  void StochasticPQnode::prepare_timestep(
      double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/
  ) {
    auto last_P = P(last_state);
    current_P = Aux::euler_maruyama_oup(
        stochasticdata->stability_parameter, stochasticdata->cut_off_factor,
        last_P, stochasticdata->theta_P, boundaryvalue(new_time)[0],
        new_time - last_time, stochasticdata->sigma_P,
        stochasticdata->distribution,
        stochasticdata->number_of_stochastic_steps);
    auto last_Q = Q(last_state);
    current_Q = Aux::euler_maruyama_oup(
        stochasticdata->stability_parameter, stochasticdata->cut_off_factor,
        last_Q, stochasticdata->theta_Q, boundaryvalue(new_time)[1],
        new_time - last_time, stochasticdata->sigma_Q,
        stochasticdata->distribution,
        stochasticdata->number_of_stochastic_steps);
    // std::cout << current_P << std::endl;
  }

  // // Version for deterministic power factor:
  // void StochasticPQnode::prepare_timestep(
  //     double last_time, double new_time,
  //     Eigen::Ref<Eigen::VectorXd const> const & last_state,
  //     Eigen::Ref<Eigen::VectorXd const> const & // new_state
  // ) {
  //   auto P_deterministic = boundaryvalue(new_time)[0];
  //   auto Q_deterministic = boundaryvalue(new_time)[1];

  //   auto last_P = P(last_state);
  //   auto last_Q = Q(last_state);
  //   current_P = Aux::euler_maruyama_oup(
  //       last_P, stochasticdata->theta_P, P_deterministic, new_time -
  //       last_time, stochasticdata->sigma_P, stochasticdata->distribution,
  //       stochasticdata->number_of_stochastic_steps);

  //   // power factor equal to deterministic power factor:
  //   if (P_deterministic != 0) {
  //     current_Q = current_P * Q_deterministic / P_deterministic;
  //   } else {
  //     current_Q = last_Q + current_P;
  //   }
  // }

  void StochasticPQnode::d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double /*last_time*/,
      double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    auto first_equation_index = get_state_startindex();
    auto second_equation_index = first_equation_index + 1;
    evaluate_P_derivative(first_equation_index, jacobianhandler, new_state);
    evaluate_Q_derivative(second_equation_index, jacobianhandler, new_state);
  }

  void StochasticPQnode::json_save(
      double time, Eigen::Ref<Eigen::VectorXd const> const &state) {
    auto P_val = current_P;
    auto Q_val = current_Q;
    auto P_deviation = P_val - boundaryvalue(time)[0];
    auto Q_deviation = Q_val - boundaryvalue(time)[1];

    nlohmann::json current_value;
    current_value["time"] = time;
    current_value["P"] = P_val;
    current_value["Q"] = Q_val;
    current_value["V"] = state[get_state_startindex()];
    current_value["phi"] = state[get_state_startindex() + 1];
    current_value["P_deviation"] = P_deviation;
    current_value["Q_deviation"] = Q_deviation;

    auto &output_json = get_output_json_ref();
    output_json["data"].push_back(std::move(current_value));
  }

  double StochasticPQnode::get_current_P() const { return current_P; }
  double StochasticPQnode::get_current_Q() const { return current_Q; }

} // namespace Model::Power
