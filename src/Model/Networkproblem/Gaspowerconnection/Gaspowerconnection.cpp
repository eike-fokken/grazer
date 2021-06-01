#include "Gaspowerconnection.hpp"
#include "Exception.hpp"
#include "ExternalPowerplant.hpp"
#include "Get_base_component.hpp"
#include "Initialvalue.hpp"
#include "Mathfunctions.hpp"
#include "Matrixhandler.hpp"
#include "Misc.hpp"
#include "Node.hpp"
#include "Powernode.hpp"
#include "make_schema.hpp"
#include <fstream>
#include <iostream>

namespace Model::Networkproblem::Gaspowerconnection {

  std::string Gaspowerconnection::get_type() { return "Gaspowerconnection"; }
  std::string Gaspowerconnection::get_gas_type() const { return get_type(); }
  nlohmann::json Gaspowerconnection::get_schema() {
    nlohmann::json schema = Network::Edge::get_schema();

    Aux::schema::add_required(
        schema, "gas2power_q_coeff", Aux::schema::type::number());
    Aux::schema::add_required(
        schema, "power2gas_q_coeff", Aux::schema::type::number());

    return schema;
  }

  std::optional<nlohmann::json> Gaspowerconnection::get_control_schema() {
    return Aux::schema::make_boundary_schema(1);
  }
  std::optional<nlohmann::json> Gaspowerconnection::get_boundary_schema() {
    return Aux::schema::make_boundary_schema(1);
  }

  nlohmann::json Gaspowerconnection::get_initial_schema() {
    std::vector<nlohmann::json> contains_x
        = {R"({"minimum": 0, "maximum": 0})"_json};
    int interpol_points = 1;
    return Aux::schema::make_initial_schema(
        interpol_points, Gaspowerconnection::init_vals_per_interpol_point(),
        contains_x);
  }

  Gaspowerconnection::Gaspowerconnection(
      nlohmann::json const &topology,
      std::vector<std::unique_ptr<Network::Node>> &nodes) :
      Network::Edge(topology, nodes),
      control(topology["control_values"]),
      boundaryvalue(topology["boundary_values"]),
      gas2power_q_coefficient(topology["gas2power_q_coeff"].get<double>()),
      power2gas_q_coefficient(topology["power2gas_q_coeff"].get<double>()) {}

  void Gaspowerconnection::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    int p_index = get_start_state_index();
    int q_index = get_start_state_index() + 1;
    rootvalues[q_index]
        = powerendnode->P(new_state) - generated_power(new_state[q_index]);
    if (is_gas_driven(new_time)) {
      rootvalues[powerendnode->get_start_state_index()]
          = new_state[p_index] - boundaryvalue(new_time)[0];
    }
  }

  void Gaspowerconnection::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    int p_index = get_start_state_index();
    int q_index = get_start_state_index() + 1;
    double q = new_state[q_index];
    jacobianhandler->set_coefficient(q_index, q_index, -dgenerated_power_dq(q));
    powerendnode->evaluate_P_derivative(q_index, jacobianhandler, new_state);

    if (is_gas_driven(new_time)) {
      jacobianhandler->set_coefficient(
          powerendnode->get_start_state_index(), p_index, 1.0);
    } else {
      jacobianhandler->set_coefficient(
          powerendnode->get_start_state_index(), p_index, 0.0);
    }
  }

  void Gaspowerconnection::setup() {

    setup_output_json_helper(get_id());

    if (powerendnode != nullptr) {
      std::cout << "You are calling setup a second time!" << std::endl;
    }
    auto powernodeptr = dynamic_cast<
        Model::Networkproblem::Gaspowerconnection::ExternalPowerplant *>(
        get_ending_node());
    if (powernodeptr == nullptr) {
      gthrow(
          {"An edge of type \"Gaspowerconnection\" can only end at an "
           "\"ExternalPowerplant\" but its ending "
           "node, ",
           get_ending_node()->get_id(), " is not an ExternalPowerplant."});
    }
    powerendnode = powernodeptr;
  }

  int Gaspowerconnection::get_number_of_states() const { return 2; }

  void Gaspowerconnection::print_to_files(nlohmann::json &new_output) {
    auto &this_output_json = get_output_json_ref();
    std::string comp_type = Aux::component_class(*this);
    new_output[comp_type][get_type()].push_back(std::move(this_output_json));
  }

  void Gaspowerconnection::json_save(
      double time, Eigen::Ref<const Eigen::VectorXd> state) {

    nlohmann::json current_value;
    current_value["time"] = time;
    current_value["pressure"] = nlohmann::json::array();
    auto start_state = get_boundary_state(1, state);
    nlohmann::json pressure0_json;
    pressure0_json["x"] = 0.0;
    pressure0_json["value"] = start_state[0];
    current_value["pressure"].push_back(pressure0_json);

    nlohmann::json flow0_json;
    flow0_json["x"] = 0.0;
    flow0_json["value"] = start_state[1];
    current_value["flow"].push_back(flow0_json);

    auto &output_json = get_output_json_ref();
    output_json["data"].push_back(std::move(current_value));
  }

  void Gaspowerconnection::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state,
      nlohmann::json const &initial_json) {
    if (get_start_state_index() == -1 or get_after_state_index() == -1) {
      gthrow(
          {"This function may only be called if set_indices  has been "
           "called beforehand!"});
    }
    // This tests whether the json is in the right format:
    auto start_p_index = get_boundary_state_index(1);
    auto start_q_index = start_p_index + 1;
    try {

      Initialvalue<2> initialvalues(initial_json);
      new_state[start_p_index] = initialvalues(0)[0];
      new_state[start_q_index] = initialvalues(0)[1];
    } catch (...) {
      std::cout << __FILE__ << ":" << __LINE__
                << ": failed to read in initial values in gaspowerconnection!"
                << std::endl;
      throw;
    }
  }

  Eigen::Vector2d Gaspowerconnection::get_boundary_p_qvol_bar(
      int direction, Eigen::Ref<Eigen::VectorXd const> state) const {
    return get_boundary_state(direction, state);
  }

  void Gaspowerconnection::dboundary_p_qvol_dstate(
      int direction, Aux::Matrixhandler *jacobianhandler,
      Eigen::RowVector2d function_derivative, int rootvalues_index,
      Eigen::Ref<Eigen::VectorXd const>) const {
    int p_index = get_boundary_state_index(direction);
    int q_index = p_index + 1;
    jacobianhandler->set_coefficient(
        rootvalues_index, p_index, function_derivative[0]);
    jacobianhandler->set_coefficient(
        rootvalues_index, q_index, function_derivative[1]);
  }

  double Gaspowerconnection::smoothing_polynomial(double q) const {
    if (q > kappa + Aux::EPSILON or q < -kappa - Aux::EPSILON) {
      gthrow(
          {" You can't call this function for values of q bigger than ",
           Aux::to_string_precise(kappa, 12), "\n"});
    }
    double rel = q / kappa;
    return q
           * (0.5 * (gas2power_q_coefficient + power2gas_q_coefficient)
              - 0.75 * (power2gas_q_coefficient - gas2power_q_coefficient) * rel
              + 0.25 * (power2gas_q_coefficient - gas2power_q_coefficient) * rel
                    * rel * rel);
  }
  double Gaspowerconnection::dsmoothing_polynomial_dq(double q) const {
    if (q > kappa + Aux::EPSILON or q < -kappa - Aux::EPSILON) {
      gthrow(
          {" You can't call this function for values of q bigger than ",
           Aux::to_string_precise(kappa, 12), "\n"});
    }
    double rel = q / kappa;
    return 0.5 * (gas2power_q_coefficient + power2gas_q_coefficient)
           - 1.5 * (power2gas_q_coefficient - gas2power_q_coefficient) * rel
           + (power2gas_q_coefficient - gas2power_q_coefficient) * rel * rel
                 * rel;
  }

  double Gaspowerconnection::generated_power(double q) const {
    if (q > kappa) {
      return gas2power_q_coefficient * q;
    } else if (q < -kappa) {
      return power2gas_q_coefficient * q;
    } else {
      return smoothing_polynomial(q);
    }
  }
  double Gaspowerconnection::dgenerated_power_dq(double q) const {
    if (q > kappa) {
      return gas2power_q_coefficient;
    } else if (q < -kappa) {
      return power2gas_q_coefficient;
    } else {
      return dsmoothing_polynomial_dq(q);
    }
  }

  bool Gaspowerconnection::is_gas_driven(double time) const {
    if (control(time)[0] > 0.0) {
      return true;
    } else {
      return false;
    }
  }

} // namespace Model::Networkproblem::Gaspowerconnection
