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

  int Gasedge::get_equation_start_index() const {
      return get_starting_state_index() + 1;// Nofstates/2;
    };

  int Gasedge::get_equation_after_index() const {
      return get_after_state_index()-1; // - Nofstates / 2 + 1 ;
    };

  int Gasedge::get_starting_state_index() const { return get_start_state_index(); };

  int Gasedge::get_ending_state_index() const {
      return get_after_state_index() - 2;
    };

    
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
} // namespace Model::Networkproblem::Gas



