#include "Pipe.hpp"
#include "Balancelaw_factory.hpp"
#include "Coloroutput.hpp"
#include "Edge.hpp"
#include "Get_base_component.hpp"
#include "Implicitboxscheme.hpp"
#include "Initialvalue.hpp"
#include "Isothermaleulerequation.hpp"
#include "Mathfunctions.hpp"
#include "Matrixhandler.hpp"
#include "Pipe_Balancelaw.hpp"
#include "Scheme_factory.hpp"
#include "Threepointscheme.hpp"
#include "make_schema.hpp"
#include "unit_conversion.hpp"

#include <Eigen/Dense>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>

namespace Model::Networkproblem::Gas {

  std::string Pipe::get_type() { return "Pipe"; }
  std::string Pipe::get_gas_type() const { return get_type(); }

  int Pipe::init_vals_per_interpol_point() { return 2; }

  namespace unit = Aux::unit;

  nlohmann::json Pipe::get_schema() {
    nlohmann::json schema = Network::Edge::get_schema();

    Aux::schema::add_required(schema, "length", unit::length.get_schema());
    Aux::schema::add_required(schema, "diameter", unit::length.get_schema());
    Aux::schema::add_required(schema, "roughness", unit::length.get_schema());

    Aux::schema::add_required(
        schema, "desired_delta_x", Aux::schema::type::number());
    Aux::schema::add_required(
        schema, "balancelaw", Aux::schema::type::string());
    Aux::schema::add_required(schema, "scheme", Aux::schema::type::string());

    return schema;
  }

  nlohmann::json Pipe::get_initial_schema() {
    std::optional<int> interpol_points = std::nullopt;
    std::vector<nlohmann::json> contains_x
        = {R"({"maximum": 0, "minimum": 0})"_json}; // there is a point <= 0
    return Aux::schema::make_initial_schema(
        interpol_points, Pipe::init_vals_per_interpol_point(), contains_x);
  }

  Pipe::Pipe(
      nlohmann::json const &topology,
      std::vector<std::unique_ptr<Network::Node>> &nodes) :
      Network::Edge(topology, nodes),
      number_of_points(
          static_cast<int>(std::ceil(
              unit::length.parse_to_si(topology["length"])
              / topology["desired_delta_x"].get<double>()))
          + 1),
      Delta_x(
          unit::length.parse_to_si(topology["length"])
          / (number_of_points - 1)),
      bl(Balancelaw::make_pipe_balancelaw(topology)),
      scheme{Scheme::make_threepointscheme(topology)} {}

  Pipe::~Pipe() {}

  void Pipe::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> last_state,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    for (int i = get_equation_start_index(); i != get_equation_after_index();
         i += 2) {

      auto rootvalue_segment = rootvalues.segment<2>(i);

      auto last_left = last_state.segment<2>(i - 1);
      auto last_right = last_state.segment<2>(i + 1);
      auto new_left = new_state.segment<2>(i - 1);
      auto new_right = new_state.segment<2>(i + 1);

      scheme->evaluate_point(
          rootvalue_segment, last_time, new_time, Delta_x, last_left,
          last_right, new_left, new_right, *bl);
    }
  }

  void Pipe::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> last_state,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    for (int i = get_equation_start_index(); i != get_equation_after_index();
         i += 2) {
      // maybe use Eigen::Ref here to avoid copies.
      auto last_left = last_state.segment<2>(i - 1);
      auto last_right = last_state.segment<2>(i + 1);
      auto new_left = new_state.segment<2>(i - 1);
      auto new_right = new_state.segment<2>(i + 1);

      Eigen::Matrix2d current_derivative_left = scheme->devaluate_point_dleft(
          last_time, new_time, Delta_x, last_left, last_right, new_left,
          new_right, *bl);

      jacobianhandler->set_coefficient(i, i - 1, current_derivative_left(0, 0));
      jacobianhandler->set_coefficient(i, i, current_derivative_left(0, 1));
      jacobianhandler->set_coefficient(
          i + 1, i - 1, current_derivative_left(1, 0));
      jacobianhandler->set_coefficient(i + 1, i, current_derivative_left(1, 1));

      Eigen::Matrix2d current_derivative_right = scheme->devaluate_point_dright(
          last_time, new_time, Delta_x, last_left, last_right, new_left,
          new_right, *bl);

      jacobianhandler->set_coefficient(
          i, i + 1, current_derivative_right(0, 0));
      jacobianhandler->set_coefficient(
          i, i + 2, current_derivative_right(0, 1));
      jacobianhandler->set_coefficient(
          i + 1, i + 1, current_derivative_right(1, 0));
      jacobianhandler->set_coefficient(
          i + 1, i + 2, current_derivative_right(1, 1));
    }
  }

  void Pipe::setup() { setup_output_json_helper(get_id()); }

  int Pipe::get_number_of_states() const { return 2 * number_of_points; }

  void Pipe::print_to_files(nlohmann::json &new_output) {
    auto &this_output_json = get_output_json_ref();
    std::string comp_type = Aux::component_class(*this);
    new_output[comp_type][get_type()].push_back(std::move(this_output_json));
  }

  void Pipe::json_save(double time, Eigen::Ref<const Eigen::VectorXd> state) {

    nlohmann::json current_value;
    current_value["time"] = time;
    for (int i = 0; i != number_of_points; ++i) {
      Eigen::Vector2d current_state
          = state.segment<2>(get_start_state_index() + 2 * i);
      double current_rho = current_state[0];
      double current_p_bar = bl->p_bar_from_p_pascal(bl->p(current_rho));
      double current_q = current_state[1];
      double x = i * Delta_x;
      nlohmann::json pressure_json;
      nlohmann::json flow_json;
      pressure_json["x"] = x;
      pressure_json["value"] = current_p_bar;
      current_value["pressure"].push_back(pressure_json);

      flow_json["x"] = x;
      flow_json["value"] = current_q;
      current_value["flow"].push_back(flow_json);
    }
    auto &output_json = get_output_json_ref();
    output_json["data"].push_back(std::move(current_value));
  }
  void Pipe::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state,
      nlohmann::json const &initial_json) {
    Initialvalue<2> initialvalues(initial_json);
    for (int i = 0; i != number_of_points; ++i) {
      try {
        new_state.segment<2>(get_start_state_index() + 2 * i)
            = bl->state(bl->p_qvol_from_p_qvol_bar(initialvalues(i * Delta_x)));
      } catch (...) {
        std::cout << "could not set initial value of pipe " << get_id() << ".\n"
                  << "Requested point was " << i * Delta_x << ". \n"
                  << "Length of line is " << get_length() << std::endl;
        std::cout << "requested - length: " << (i * Delta_x - get_length())
                  << std::endl;
        throw;
      }
    }
  }

  Eigen::Vector2d Pipe::get_boundary_p_qvol_bar(
      int direction, Eigen::Ref<Eigen::VectorXd const> state) const {
    Eigen::Vector2d b_state = get_boundary_state(direction, state);
    Eigen::Vector2d p_qvol = bl->p_qvol(b_state);
    return bl->p_qvol_bar_from_p_qvol(p_qvol);
  }

  void Pipe::dboundary_p_qvol_dstate(
      int direction, Aux::Matrixhandler *jacobianhandler,
      Eigen::RowVector2d function_derivative, int rootvalues_index,
      Eigen::Ref<Eigen::VectorXd const> state) const {
    Eigen::Vector2d boundary_state = get_boundary_state(direction, state);

    Eigen::Vector2d p_qvol = bl->p_qvol(boundary_state);

    Eigen::Matrix2d dp_qvol_dstate = bl->dp_qvol_dstate(boundary_state);
    Eigen::Matrix2d dpqvolbar_dpqvol
        = bl->dp_qvol_bar_from_p_qvold_p_qvol(p_qvol);

    Eigen::Matrix2d dpqvol_bar_dstate = dpqvolbar_dpqvol * dp_qvol_dstate;

    Eigen::RowVector2d derivative;
    derivative = function_derivative * dpqvol_bar_dstate;

    int rho_index = get_boundary_state_index(direction);
    int q_index = rho_index + 1;
    jacobianhandler->set_coefficient(
        rootvalues_index, rho_index, derivative[0]);
    jacobianhandler->set_coefficient(rootvalues_index, q_index, derivative[1]);
  }

  double Pipe::get_length() { return (number_of_points - 1) * Delta_x; }

} // namespace Model::Networkproblem::Gas
