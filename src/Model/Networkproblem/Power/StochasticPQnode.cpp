#include "StochasticPQnode.hpp"
#include "Powernode.hpp"
#include "Stochastic.hpp"
#include "make_schema.hpp"
#include <fstream>

namespace Model::Networkproblem::Power {

  std::string StochasticPQnode::get_type() { return "StochasticPQnode"; }
  nlohmann::json StochasticPQnode::get_schema() {
    nlohmann::json schema = Powernode::get_schema();
    Aux::schema::add_required(schema, "sigma_P", Aux::schema::type::number());
    Aux::schema::add_required(schema, "theta_P", Aux::schema::type::number());
    Aux::schema::add_required(schema, "sigma_Q", Aux::schema::type::number());
    Aux::schema::add_required(schema, "theta_Q", Aux::schema::type::number());

    return schema;
  }

  StochasticPQnode::StochasticPQnode(nlohmann::json const &topology) :
      Powernode(topology),
      stochasticdata(std::make_unique<StochasticData>(
          topology["sigma_P"], topology["theta_P"], topology["sigma_Q"],
          topology["theta_Q"], topology["number_of_stochastic_steps"])) {
    // std::cout << stochasticdata->number_of_stochastic_steps << std::endl;
    // std::cout << stochasticdata->theta_P << std::endl;
    // std::cout << stochasticdata->sigma_P << std::endl;
    // std::cout << stochasticdata->theta_Q << std::endl;
    // std::cout << stochasticdata->sigma_Q << std::endl;
  }

  void StochasticPQnode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const> // last_state
      ,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    auto V_index = get_start_state_index();
    auto phi_index = V_index + 1;
    rootvalues[V_index] = P(new_state) - current_P;
    rootvalues[phi_index] = Q(new_state) - current_Q;
  }

  void StochasticPQnode::prepare_timestep(
      double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> last_state,
      Eigen::Ref<Eigen::VectorXd const> // new_state
  ) {
    auto last_P = P(last_state);
    current_P = Aux::euler_maruyama(
        last_P, stochasticdata->theta_P, boundaryvalue(new_time)[0],
        new_time - last_time, stochasticdata->sigma_P,
        stochasticdata->distribution,
        stochasticdata->number_of_stochastic_steps);
    auto last_Q = Q(last_state);
    current_Q = Aux::euler_maruyama(
        last_Q, stochasticdata->theta_Q, boundaryvalue(new_time)[1],
        new_time - last_time, stochasticdata->sigma_Q,
        stochasticdata->distribution,
        stochasticdata->number_of_stochastic_steps);
    // std::cout << current_P << std::endl;
  }

  void StochasticPQnode::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler,
      double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    auto first_equation_index = get_start_state_index();
    auto second_equation_index = first_equation_index + 1;
    evaluate_P_derivative(first_equation_index, jacobianhandler, new_state);
    evaluate_Q_derivative(second_equation_index, jacobianhandler, new_state);
  }

  void StochasticPQnode::save_values(
      double time, Eigen::Ref<Eigen::VectorXd const> state) {

    auto P_val = current_P;
    auto Q_val = current_Q;
    auto P_deviation = P_val - boundaryvalue(time)[0];
    auto Q_deviation = Q_val - boundaryvalue(time)[1];
    std::map<double, double> Pmap;
    std::map<double, double> Qmap;
    std::map<double, double> Vmap;
    std::map<double, double> phimap;
    std::map<double, double> P_deviation_map;
    std::map<double, double> Q_deviation_map;

    std::vector<std::map<double, double>> value_vector;
    Pmap = {{0.0, P_val}};
    Qmap = {{0.0, Q_val}};
    Vmap = {{0.0, state[get_start_state_index()]}};
    phimap = {{0.0, state[get_start_state_index() + 1]}};
    P_deviation_map = {{0.0, P_deviation}};
    Q_deviation_map = {{0.0, Q_deviation}};
    value_vector = {Pmap, Qmap, Vmap, phimap, P_deviation_map, Q_deviation_map};
    Equationcomponent::push_to_values(time, value_vector);
  }

  void StochasticPQnode::print_to_files(
      std::filesystem::path const &output_directory) {
    std::filesystem::path node_output_directory(
        output_directory / (get_id_copy().insert(0, "Power_")));

    std::ofstream output(node_output_directory);

    output
        << "time,    P,    Q,    V,    phi,    P_deviation,    Q_deviation\n";
    auto values = get_values();
    output.precision(9);
    auto times = get_times();

    for (unsigned long i = 0; i != times.size(); ++i) {
      output << times[i];
      for (auto const &var : values[i]) { output << ",    " << var.at(0.0); }
      output << "\n";
    }
    output << "\n";
  }

} // namespace Model::Networkproblem::Power
