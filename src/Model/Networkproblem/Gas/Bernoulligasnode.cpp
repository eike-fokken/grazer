#include "Bernoulligasnode.hpp"
#include "Coloroutput.hpp"
#include "Edge.hpp"
#include "Exception.hpp"
#include "Gasedge.hpp"
#include "Matrixhandler.hpp"
#include "Pipe.hpp"
#include <iostream>

namespace Model::Networkproblem::Gas {

  void Bernoulligasnode::evaluate_flow_node_balance(Eigen::Ref<Eigen::VectorXd> rootvalues,
                                           Eigen::Ref<Eigen::VectorXd const> const &state,
                                           double prescribed_qvol) const {

    if(directed_attached_gas_edges.empty()){ return; }
    auto [dir0,edge0] = directed_attached_gas_edges.front();
    auto p_qvol0 = edge0->get_boundary_p_qvol_bar(dir0,state);
    auto p0 = p_qvol0[0];
    auto q0 = p_qvol0[1];
    

    double old_p=p0;
    int old_equation_index = edge0->give_away_boundary_index(dir0);

    // We will write the flow balance into the last index:
    int last_direction = directed_attached_gas_edges.back().first;
    int last_equation_index = directed_attached_gas_edges.back().second->give_away_boundary_index(last_direction);
    

    // prescribed boundary condition is like an attached pipe ending at this node...
    rootvalues[last_equation_index] = -1.0* prescribed_qvol; 
    rootvalues[last_equation_index] += dir0*q0;


    // std::cout << "number of gas edges: " << directed_attached_gas_edges.size() <<std::endl;
    for(auto it=std::next(directed_attached_gas_edges.begin());it!=directed_attached_gas_edges.end();++it){
      int direction=it->first;
      Gasedge *edge = it->second;
      auto current_p_qvol = edge->get_boundary_p_qvol_bar(direction,state);
      auto current_p=current_p_qvol[0];
      auto current_qvol=current_p_qvol[1];
      rootvalues[old_equation_index] = current_p-old_p;
      old_equation_index=edge->give_away_boundary_index(direction);
      old_p=current_p;


      rootvalues[last_equation_index]+=direction*current_qvol;
    }
  }


  void Bernoulligasnode::evaluate_flow_node_derivative(Aux::Matrixhandler * jacobianhandler, Eigen::Ref<Eigen::VectorXd const> const & state) const {

    if(directed_attached_gas_edges.empty()){ return; }

    auto [dir0,edge0] =directed_attached_gas_edges.front();
    auto [dirlast,edgelast] = directed_attached_gas_edges.back();
    int last_equation_index = edgelast->give_away_boundary_index(dirlast);
    //    rootvalues[last_equation_index]=dir0*state0[1];


      
      Eigen::RowVector2d dF_last_dpq_0(0.0, dir0 );
      edge0->dboundary_p_qvol_dstate(dir0,jacobianhandler, dF_last_dpq_0, last_equation_index, state);
      // if there is only one attached edge, we are done:
      if(edge0==edgelast){return;}

      // In all other cases we now have to make pressure derivatives and the other flow derivatives:


      // first edge is special (sets only one p-derivative)
      Eigen::RowVector2d dF_0_dpq_0(-1.0,0.0);
      int old_equation_index = edge0->give_away_boundary_index(dir0);
      
      edge0->dboundary_p_qvol_dstate(dir0,jacobianhandler, dF_0_dpq_0, old_equation_index, state);

      //first and last attached edge are special:
      auto second_iterator = std::next(directed_attached_gas_edges.begin());
      auto last_iterator = std::prev(directed_attached_gas_edges.end());
    for(auto it=second_iterator;it!=last_iterator;++it){
      int direction=it->first;
      Gasedge *edge = it->second;

      int current_equation_index = edge->give_away_boundary_index(direction);
      Eigen::RowVector2d dF_old_dpq_now(1.0,0.0);  
      Eigen::RowVector2d dF_now_dpq_now(-1.0,0.0);
      Eigen::RowVector2d dF_last_dpq_now(0.0, direction);

      // Let the attached edge write out the derivative:
      edge->dboundary_p_qvol_dstate(direction,jacobianhandler, dF_old_dpq_now, old_equation_index, state);
      edge->dboundary_p_qvol_dstate(direction,jacobianhandler, dF_now_dpq_now, current_equation_index, state);
      edge->dboundary_p_qvol_dstate(direction,jacobianhandler, dF_last_dpq_now, last_equation_index, state);
      old_equation_index=current_equation_index;
    }
    //last edge:
    Eigen::RowVector2d dF_old_dpq_last(1.0,0.0);
    edgelast->dboundary_p_qvol_dstate(dirlast,jacobianhandler, dF_old_dpq_last, old_equation_index, state);
    Eigen::RowVector2d dF_last_dpq_last(0.0, dirlast);
    edgelast->dboundary_p_qvol_dstate(dirlast,jacobianhandler, dF_last_dpq_last, last_equation_index, state);
  }

  void Bernoulligasnode::setup() {

    if(directed_attached_gas_edges.size()!=0){std::cout << YELLOW << "You are calling setup a second time!" << RESET <<std::endl;}


    directed_attached_gas_edges.clear();

    //Treat pipes separately for bernoulli coupling:
    std::vector<std::pair<int, Gasedge *>> directed_pipes;
    std::vector<std::pair<int, Gasedge *>> directed_other_edges;

    for (auto &startedge : get_starting_edges()) {
      auto startgasedge = dynamic_cast<Gasedge *>(startedge);
      if (!startgasedge) {
        std::cout << __FILE__ << ":" << __LINE__ << " Warning: The non-gasedge"
                  << startedge->get_id() << " is attached at node " << get_id()
                  << std::endl;
        continue;
      }
      auto * is_pipe=dynamic_cast<Pipe*>(startgasedge);
      if(is_pipe){
        directed_pipes.push_back({1,startgasedge});
      } else {
      directed_other_edges.push_back({1, startgasedge});
      }
    }
    for (auto &endedge : get_ending_edges()) {
      auto endgasedge = dynamic_cast<Gasedge *>(endedge);
      if(!endgasedge) {
        std::cout << __FILE__ <<":"<< __LINE__ << " Warning: The non-gasedge" << endedge->get_id() << " is attached at node " << get_id()  << std::endl;
        std::cout << "node id: " << get_id() << std::endl;
        continue;}
      auto *is_pipe = dynamic_cast<Pipe *>(endgasedge);
      if (is_pipe) {
        directed_pipes.push_back({-1, endgasedge});
      } else {
        directed_other_edges.push_back({-1, endgasedge});
      }
    }

    end_of_pipe_indices=static_cast<int>(directed_pipes.size());

    directed_attached_gas_edges = directed_pipes;
    directed_attached_gas_edges.insert(directed_attached_gas_edges.end(),
                                       directed_other_edges.begin(),
                                       directed_other_edges.end());
    // Notify the user of unconnected nodes:
    if(directed_attached_gas_edges.empty()){
      std::cout << "Node " <<get_id() << " has no attached gas edges!" << std::endl;
      return; }
  }

  int Bernoulligasnode::get_number_of_states() const { return number_of_state_variables;}

} // namespace Model::Networkproblem::Gas
