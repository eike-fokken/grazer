#include "Gaspowerconnection.hpp"
#include "Exception.hpp"
#include "Mathfunctions.hpp"
#include "Matrixhandler.hpp"
#include "Misc.hpp"
#include "Node.hpp"
#include "Powernode.hpp"
#include <fstream>
#include <iostream>

namespace Model::Networkproblem::Gas {

  std::string Gaspowerconnection::get_type() { return "Gaspowerconnection"; }

  Gaspowerconnection::Gaspowerconnection(
      nlohmann::json const &topology,
      std::vector<std::unique_ptr<Network::Node>> &nodes)
      : Network::Edge(topology, nodes),
        gas2power_q_coefficient(
            std::stod(topology["gas2power_q_coeff"].get<std::string>())),
        power2gas_q_coefficient(
            std::stod(topology["power2gas_q_coeff"].get<std::string>())) {}


  void Gaspowerconnection::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    int q_index = get_start_state_index() + 1;
    rootvalues[q_index] = powerendnode->P(new_state) - generated_power(new_state[q_index]);
  }

  void Gaspowerconnection::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler, double , double ,
                                                     Eigen::Ref<Eigen::VectorXd const> const &, Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    int q_index = get_start_state_index() + 1;
    double q = new_state[q_index];
    jacobianhandler->set_coefficient(q_index, q_index, - dgenerated_power_dq(q));
    powerendnode->evaluate_P_derivative(q_index, jacobianhandler, new_state);
  }

  void Gaspowerconnection::setup(){
    if(powerendnode != nullptr){std::cout << "You are calling setup a second time!"<<std::endl;}
    auto powernodeptr = dynamic_cast<Model::Networkproblem::Power::Powernode *>(
        get_ending_node());
    if (powernodeptr == nullptr) {
      gthrow({"An edge of type \"Gaspowerconnection\" can only end at a \"Powernode\" but its ending "
              "node, ",
              get_ending_node()->get_id(), " is not a Powernode."});
    }
    powerendnode = powernodeptr;
  }

  int Gaspowerconnection::get_number_of_states() const { return 2;}

  void
  Gaspowerconnection::print_to_files(std::filesystem::path const &output_directory) {
    std::string pressure_file_name = (get_id_copy().insert(0, "Gas_Gaspowerconnection_"))+"_p";
    std::string flow_file_name =(get_id_copy().insert(0, "Gas_Gaspowerconnection_"))+"_q";
    std::filesystem::path shortpipe_output_pressure(output_directory /
                                                    pressure_file_name);
    std::filesystem::path shortpipe_output_flow(
                                                output_directory / flow_file_name);

    std::ofstream outputpressure(shortpipe_output_pressure);
    std::ofstream outputflow(shortpipe_output_flow);

    outputpressure << "t-x,    0.0\n";
    outputflow << "t-x,    0.0\n";
    auto times = get_times();
    auto values = get_values();

    outputpressure.precision(9);
    outputflow.precision(9);

    for (unsigned i = 0; i != times.size(); ++i) {
      {
        // write out pressures:
        auto var = values[i][0];
        outputpressure << times[i];
        outputpressure << ",    " << var.at(0.0);
        outputpressure << std::endl;
      }
      {
        // write out flows:
        outputflow << times[i];
        auto var = values[i][1];
        outputflow << ",    " << var.at(0.0);
        outputflow << std::endl;
      }
    }
  }

  void Gaspowerconnection::save_values(double time, Eigen::Ref<Eigen::VectorXd const> const &state) {
      std::map<double, double> pressure_map;
      std::map<double, double> flow_map;

      auto start_state = get_boundary_state(1, state);
      pressure_map = {{0.0, start_state[0]}};
      flow_map = {{0.0, start_state[1]}};

      std::vector<std::map<double, double>> value_vector(
          {pressure_map, flow_map});
      Equationcomponent::push_to_values(time, value_vector);
  }
  

  void Gaspowerconnection::set_initial_values(Eigen::Ref<Eigen::VectorXd>new_state,
                                              nlohmann::ordered_json initial_json) {
    if (get_start_state_index() == -1 or get_after_state_index() == -1) {
      gthrow({"This function may only be called if set_indices  has been "
              "called beforehand!"});
    }
    // This tests whether the json is in the right format:
    try {
      if ( (!initial_json["data"][0]["values"].is_array()) or
          initial_json["data"][0]["values"].size() != 2) {
        std::cout << "The initial json for this gaspowerconnection is given by:"
                  << "\n";
        std::cout << initial_json << std::endl;
        gthrow({"This is not a gaspowerconnection initial condition!"});
      }
    } catch (...) {
      std::cout << __FILE__ << ":" << __LINE__
                << " The exception was thrown here." << std::endl;
      throw;
    }
    auto start_p_index = get_boundary_state_index(1);
    auto start_q_index = start_p_index + 1;
    try {
      new_state[start_p_index] = initial_json["data"][0]["values"][0];
      new_state[start_q_index] = initial_json["data"][0]["values"][1];
    } catch (...) {
      std::cout << __FILE__ << ":" << __LINE__
                << ": failed to read in initial values in gaspowerconnection!"
                << std::endl;
      throw;
    }
  }

  Eigen::Vector2d Gaspowerconnection::get_boundary_p_qvol_bar(
      int direction, Eigen::Ref<Eigen::VectorXd const> const &state) const {
    return get_boundary_state(direction, state);
  }

  void Gaspowerconnection::dboundary_p_qvol_dstate(
      int direction, Aux::Matrixhandler *jacobianhandler,
      Eigen::RowVector2d function_derivative, int rootvalues_index,
      Eigen::Ref<Eigen::VectorXd const> const &) const {
    int p_index = get_boundary_state_index(direction);
    int q_index = p_index + 1;
    jacobianhandler->set_coefficient(rootvalues_index, p_index,
                                     function_derivative[0]);
    jacobianhandler->set_coefficient(rootvalues_index, q_index,
                                     function_derivative[1]);
  }

  


  double Gaspowerconnection::smoothing_polynomial(double q) const {
    if (q > kappa + Aux::EPSILON or q < -kappa - Aux::EPSILON) {
      gthrow({" You can't call this function for values of q bigger than ",
              Aux::to_string_precise(kappa, 12), "\n"});
    }
    double rel = q / kappa;
    return q * (0.5 * (gas2power_q_coefficient + power2gas_q_coefficient) - 0.75 * (power2gas_q_coefficient - gas2power_q_coefficient) * rel +
                0.25 * (power2gas_q_coefficient - gas2power_q_coefficient) * rel * rel * rel);
  }
  double Gaspowerconnection::dsmoothing_polynomial_dq(double q) const {
    if(q>kappa+Aux::EPSILON or q < -kappa-Aux::EPSILON) {
      gthrow({" You can't call this function for values of q bigger than ", Aux::to_string_precise(kappa, 12), "\n"});
    }
    double rel = q / kappa;
    return 0.5 * (gas2power_q_coefficient + power2gas_q_coefficient) - 1.5 * (power2gas_q_coefficient - gas2power_q_coefficient) * rel +
           (power2gas_q_coefficient - gas2power_q_coefficient) * rel * rel * rel;
  }

  double Gaspowerconnection::generated_power(double q) const {
    if (q > kappa) {
      return  gas2power_q_coefficient * q;
    } else if (q <- kappa) {
      return power2gas_q_coefficient * q;
    } else {
      return smoothing_polynomial(q);
    }

  }
  double Gaspowerconnection::dgenerated_power_dq(double q) const{
    if (q > kappa) {
      return gas2power_q_coefficient;
    } else if (q < -kappa) {
      return power2gas_q_coefficient;
    } else {
      return dsmoothing_polynomial_dq(q);
    }
  }

} // namespace Model::Networkproblem::Gas
