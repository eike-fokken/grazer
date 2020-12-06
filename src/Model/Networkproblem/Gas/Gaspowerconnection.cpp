#include <Gaspowerconnection.hpp>


namespace Model::Networkproblem::Gas {

  void Gaspowerconnection::evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
                                    double new_time, Eigen::VectorXd const &last_state,
                                    Eigen::VectorXd const &new_state) const {}
  void Gaspowerconnection::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
                                                     Eigen::VectorXd const &, Eigen::VectorXd const &new_state) const {}


  int Gaspowerconnection::get_number_of_states() const { return 2;}

  void
  Gaspowerconnection::print_to_files(std::filesystem::path const &output_directory) {}

  void Gaspowerconnection::save_values(double time, Eigen::VectorXd const &state) {}

  void Gaspowerconnection::set_initial_values(Eigen::VectorXd &new_state,
                                              nlohmann::ordered_json initial_json) {}


  Eigen::Vector2d Gaspowerconnection::get_boundary_p_qvol(int direction, Eigen::VectorXd const &state) const {}


  void Gaspowerconnection::dboundary_p_qvol_dstate(int direction, Aux::Matrixhandler * jacobianhandler, Eigen::RowVector2d function_derivative, int rootvalues_index, Eigen::VectorXd const &state) const {}


}
