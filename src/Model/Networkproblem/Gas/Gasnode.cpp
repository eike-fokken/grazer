#include <Matrixhandler.hpp>
#include <Gasedge.hpp>
#include <Gasnode.hpp>
#include <iostream>

namespace Model::Networkproblem::Gas {

  void Gasnode::evaluate_flow_node_balance(Eigen::VectorXd &rootvalues,
                                           Eigen::VectorXd const &state,
                                           double prescribed_flow) const {

    if(directed_attached_gas_edges.empty()){
      std::cout << "Node " <<get_id() << " has no attached gas edges!" << std::endl;
      return;
    }
    auto [dir0,edge0] =directed_attached_gas_edges.front();
    auto state0 = edge0->get_boundary_state(dir0,state);
    double old_pressure=state0[0];

    int old_equation_index = edge0->give_away_boundary_index(dir0);
    int last_direction = directed_attached_gas_edges.back().first;
    int last_equation_index = directed_attached_gas_edges.back().second->give_away_boundary_index(last_direction);
    rootvalues[last_equation_index] = - prescribed_flow;
    rootvalues[last_equation_index]+= dir0*state0[1];

    for(auto it=std::next(directed_attached_gas_edges.begin());it!=directed_attached_gas_edges.end();++it){
      int direction=it->first;
      Gasedge *edge = it->second;
      auto current_state = edge->get_boundary_state(direction,state);
      auto current_pressure=current_state[0];
      auto current_flow=current_state[1];

      rootvalues[old_equation_index] = current_pressure-old_pressure;
      old_equation_index=edge->give_away_boundary_index(direction);
      old_pressure=current_pressure;

      rootvalues[last_equation_index]+=direction*current_flow;
    }
  }

  void Gasnode::evaluate_pressure_node_balance(Eigen::VectorXd & rootvalues, Eigen::VectorXd const & state, double prescribed_pressure) const {

    if(directed_attached_gas_edges.empty()){
      std::cout << "Node " <<get_id() << " has no attached gas edges!" << std::endl;
      return;
    }

    for(auto & [direction,edge] : directed_attached_gas_edges){
      auto current_state = edge->get_boundary_state(direction,state);
      double current_pressure=current_state[0];
      int equation_index = edge->give_away_boundary_index(direction);
      rootvalues[equation_index] = current_pressure - prescribed_pressure;
    }

  }



  /// This function sets pressure equality boundary conditions.
  
  void Gasnode::evaluate_flow_node_derivative(Aux::Matrixhandler * jacobianhandler, Eigen::VectorXd const & ) const {

    if(directed_attached_gas_edges.empty()){
      std::cout << "Node " <<get_id() << " has no attached gas edges!" << std::endl;
      return;
    }
    auto [dir0,edge0] =directed_attached_gas_edges.front();
    int old_p_index = edge0->get_boundary_state_index(dir0);
    int old_q_index = old_p_index+1;
    int old_equation_index = edge0->give_away_boundary_index(dir0);
    int last_direction = directed_attached_gas_edges.back().first;
    int last_equation_index = directed_attached_gas_edges.back().second->give_away_boundary_index(last_direction);
    //    rootvalues[last_equation_index]=dir0*state0[1];
    jacobianhandler->set_coefficient(last_equation_index,old_q_index,dir0);

    for(auto it=std::next(directed_attached_gas_edges.begin());it!=directed_attached_gas_edges.end();++it){
      int direction=it->first;
      Gasedge *edge = it->second;
      int current_p_index = edge->get_boundary_state_index(direction);
      int current_q_index = current_p_index+1;
          //      rootvalues[old_equation_index] = current_pressure-old_pressure;
      jacobianhandler->set_coefficient(old_equation_index,current_p_index,1.0);
      jacobianhandler->set_coefficient(old_equation_index,old_p_index,-1.0);
      old_equation_index=edge->give_away_boundary_index(direction);
      old_p_index= current_p_index;
      
      jacobianhandler->set_coefficient(old_equation_index,current_q_index,direction);
    }
    
  }

  void Gasnode::evaluate_pressure_node_derivative(Aux::Matrixhandler * jacobianhandler, Eigen::VectorXd const & ) const {

    if(directed_attached_gas_edges.empty()){
      std::cout << "Node " <<get_id() << " has no attached gas edges!" << std::endl;
      return;
    }
    for (auto & [direction, edge] : directed_attached_gas_edges) {
      int current_p_index = edge->get_boundary_state_index(direction);
      int equation_index = edge->give_away_boundary_index(direction);
      jacobianhandler->set_coefficient(equation_index, current_p_index, 1.0);
    }
    
    }

  void Gasnode::setup() {
    for (auto &startedge : get_starting_edges()) {
      auto startgasedge = dynamic_cast<Gasedge *>(startedge);
      if(!startgasedge) {
        std::cout << __FILE__ <<":"<< __LINE__ << " Warning: The non-gasedge" << startedge->get_id() << " is attached at node " << get_id()  << std::endl;
        std::cout << "node id: " << get_id() << std::endl;
        continue;}
      directed_attached_gas_edges.push_back({1,startgasedge});
    }
    for (auto &endedge : get_ending_edges()) {
      auto endgasedge = dynamic_cast<Gasedge *>(endedge);
      if(!endgasedge) {
        std::cout << __FILE__ <<":"<< __LINE__ << " Warning: The non-gasedge" << endedge->get_id() << " is attached at node " << get_id()  << std::endl;
        std::cout << "node id: " << get_id() << std::endl;
        continue;}
      directed_attached_gas_edges.push_back({-1,endgasedge});
    }
  }

  int Gasnode::get_number_of_states() const { return 0;}

} // namespace Model::Networkproblem::Gas
