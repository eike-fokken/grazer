#include <Matrixhandler.hpp>
#include <Vphinode.hpp>
#include <iostream>

namespace Model::Networkproblem::Power {

  std::string Vphinode::get_type() { return "Vphinode"; }
  bool Vphinode::needs_boundary_values() { return true; }

  void Vphinode::evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double // last_time
                          ,
                          double new_time,
                          Eigen::Ref<Eigen::VectorXd const> const & // last_state
                          ,
                          Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    rootvalues[V_index] = new_state[V_index] - boundaryvalue(new_time)[0];

    rootvalues[phi_index] = new_state[phi_index] - boundaryvalue(new_time)[1];
  }

  void Vphinode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                           double // last_time
                                           ,
                                           double // new_time
                                           ,
                                           Eigen::Ref<Eigen::VectorXd const> const &,
                                           Eigen::Ref<Eigen::VectorXd const> const & // new_state
  ) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    jacobianhandler->set_coefficient(V_index, V_index, 1.0);
    jacobianhandler->set_coefficient(phi_index, phi_index, 1.0);
  }

  void Vphinode::save_values(double time,
                   Eigen::Ref<Eigen::VectorXd const> const &state){
    auto P_val = P(state);
    auto Q_val = Q(state);
    save_power_values(time, state, P_val, Q_val)  ;
  }

  void Vphinode::display() const {
    Node::print_id();
    Equationcomponent::print_indices();
    std::cout << "type: Vphi, G: " << G << ", B: " << B << "\n";
  }

} // namespace Model::Networkproblem::Power
