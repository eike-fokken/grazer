#include <Gasedge.hpp>
#include <Exception.hpp>

namespace Model::Networkproblem::Gas {

  int Gasedge::give_away_start_index() {
      if (get_start_state_index()<0 or get_after_state_index()<0) {
        gthrow({"This function: ", __FUNCTION__, " can only be called after set_indices(...) has been called."});
      }
      return get_start_state_index();
    }

  int Gasedge::give_away_end_index(){
      if (get_start_state_index() < 0 or get_after_state_index() < 0) {
        gthrow({"This function: ", __FUNCTION__,
                " can only be called after set_indices(...) has been called."});
      }
      return (get_after_state_index()-1);
    };
  int Gasedge::give_away_boundary_index(int direction){
    if(direction==1){
      return give_away_start_index();
    } else if (direction==-1){
      return give_away_end_index();
    } else {
      gthrow({"The supplied direction was ", std::to_string(direction), ", which is not +1 or -1! Edge id is: ", get_id()});
    }
  }



  int Gasedge::get_equation_start_index() const {
      return get_starting_state_index() + 1;// Nofstates/2;
    }
  int Gasedge::get_equation_after_index() const {
      return get_after_state_index()-1; // - Nofstates / 2 + 1 ;
    }

  int Gasedge::get_starting_state_index() const { return get_start_state_index(); };
  int Gasedge::get_ending_state_index() const {
      return get_after_state_index() - 2;
    }
  int Gasedge::get_boundary_state_index(int direction) const{
    if(direction==1){
      return get_starting_state_index();
    } else if (direction==-1){
      return get_ending_state_index();
    } else {
      gthrow({"The supplied direction was ", std::to_string(direction), ", which is not +1 or -1! Edge id is: ", get_id()});
    }
  }
      Eigen::Vector2d
      Gasedge::get_starting_state(Eigen::VectorXd const &state) const {
        Eigen::Vector2d starting_state =
          state.segment<2>(get_starting_state_index());
      return starting_state;
    }

    
    Eigen::Vector2d
    Gasedge::get_ending_state(Eigen::VectorXd const &state) const {
      Eigen::Vector2d ending_state =
          state.segment<2>(get_ending_state_index());
      return ending_state;
    };
  Eigen::Vector2d Gasedge::get_boundary_state(int direction, Eigen::VectorXd const &state) const{
    if(direction==1){
      return get_starting_state(state);
    } else if (direction==-1){
      return get_ending_state(state);
    } else {
      gthrow({"The supplied direction was ", std::to_string(direction), ", which is not +1 or -1! Edge id is: ", get_id()});
    }
  }
} // namespace Model::Networkproblem::Gas



