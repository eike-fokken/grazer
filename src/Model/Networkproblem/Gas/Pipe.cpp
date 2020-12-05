#include <Implicitboxscheme.hpp>
#include <Isothermaleulerequation.hpp>
#include <Matrixhandler.hpp>
#include <Eigen/Dense>
#include <Pipe.hpp>
#include <cmath>
#include <Mathfunctions.hpp>

namespace Model::Networkproblem::Gas {

  Pipe::Pipe(std::string _id, Network::Node *start_node,
             Network::Node *end_node, nlohmann::ordered_json topology_json,
             double _Delta_x)
      : Gasedge(_id, start_node, end_node),
        length(topology_json["length"]["value"].get<double>() * 1e3),
        diameter(topology_json["diameter"]["value"].get<double>() * 1e-3),
        roughness(topology_json["roughness"]["value"].get<double>()),
        number_of_points(static_cast<int> (std::ceil(length/_Delta_x))+1),
        Delta_x(length/number_of_points),
        bl(Balancelaw::Isothermaleulerequation(Aux::circle_area(0.5 * diameter),
                                               diameter, roughness)) {}


  int Pipe::get_number_of_states() const {
    return 2*number_of_points;
  }





  Eigen::Vector2d
  Pipe::get_boundary_p_qvol(int direction, Eigen::VectorXd const &state) const {
    Eigen::Vector2d b_state = get_boundary_state( direction, state);

    return bl.p_qvol(b_state);
  }

  void Pipe::dboundary_p_qvol_dstate(int direction,
                               Aux::Matrixhandler *jacobianhandler,
                               Eigen::RowVector2d function_derivative,
                               int rootvalues_index,
                                     Eigen::VectorXd const &state) const {
    Eigen::RowVector2d derivative;
    derivative = function_derivative *
                 bl.dp_qvol_dstate(get_boundary_state(direction, state));
    int rho_index = get_boundary_state_index(direction);
    int q_index = rho_index + 1;
    jacobianhandler->set_coefficient(rootvalues_index, rho_index,
                                     derivative[0]);
    jacobianhandler->set_coefficient(rootvalues_index, q_index, derivative[1]);
  }

} // namespace Model::Networkproblem::Gas
