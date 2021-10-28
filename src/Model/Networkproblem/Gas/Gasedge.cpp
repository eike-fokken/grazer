#include "Gasedge.hpp"
#include "Exception.hpp"
#include "Idobject.hpp"
#include "make_schema.hpp"

namespace Model::Gas {

  std::string Direction_string(Direction direction) {
    if (direction == start) {
      return "start";
    } else if (direction == end) {
      return "end";
    } else {
      gthrow(
          {"The enum direction can only be start or end! Someone changed its "
           "definition!"});
    }
  }

  int Gasedge::init_vals_per_interpol_point() { return 2; }

  Eigen::Index Gasedge::give_away_start_index() const {
    if (get_state_startindex() < 0 or get_state_afterindex() < 0) {
      gthrow(
          {"This function: ", __FUNCTION__,
           " can only be called after set_state_indices(...) has been "
           "called."});
    }
    return get_state_startindex();
  }

  Eigen::Index Gasedge::give_away_end_index() const {
    if (get_state_startindex() < 0 or get_state_afterindex() < 0) {
      gthrow(
          {"This function: ", __FUNCTION__,
           " can only be called after set_state_indices(...) has been "
           "called."});
    }
    return (get_state_afterindex() - 1);
  }
  Eigen::Index Gasedge::boundary_equation_index(Direction direction) const {
    if (direction == start) {
      return give_away_start_index();
    } else if (direction == end) {
      return give_away_end_index();
    } else {
      auto *this_idobject = dynamic_cast<Network::Idobject const *>(this);
      if (!this_idobject) {
        gthrow(
            {"This gasedge is not and Idobject, which should never happen!"});
      }
      gthrow(
          {"The supplied direction was ", Direction_string(direction),
           ", which is neither \"start\" nor \"end\"! Edge id is: ",
           this_idobject->get_id()});
    }
  }

  Eigen::Index Gasedge::get_equation_start_index() const {
    return get_starting_state_index() + 1; // Nofstates/2;
  }
  Eigen::Index Gasedge::get_equation_after_index() const {
    return get_state_afterindex() - 1; // - Nofstates / 2 + 1 ;
  }

  Eigen::Index Gasedge::get_starting_state_index() const {
    return get_state_startindex();
  }

  Eigen::Index Gasedge::get_ending_state_index() const {

    // This is a hack and should be refactored
    if (get_state_afterindex() - get_state_startindex() == 2) {
      auto *this_idobject = dynamic_cast<Network::Idobject const *>(this);
      if (!this_idobject) {
        gthrow(
            {"This gasedge is not and Idobject, which should never happen!"});
      }
      gthrow(
          {"Edge: ", this_idobject->get_id(),
           " has only two variables, therefore this function should not have "
           "been called!"});
    }

    return get_state_afterindex() - 2;
  }

  Eigen::Index Gasedge::get_boundary_state_index(Direction direction) const {
    if (direction == start) {
      return get_starting_state_index();
    } else if (direction == end) {
      return get_ending_state_index();
    } else {
      auto *this_idobject = dynamic_cast<Network::Idobject const *>(this);
      if (!this_idobject) {
        gthrow(
            {"This gasedge is not and Idobject, which should never happen!"});
      }
      gthrow(
          {"The supplied direction was ", std::to_string(direction),
           ", which is not +1 or -1! Edge id is: ", this_idobject->get_id()});
    }
  }
  Eigen::Vector2d Gasedge::get_starting_state(
      Eigen::Ref<Eigen::VectorXd const> const &state) const {
    Eigen::Vector2d starting_state
        = state.segment<2>(get_starting_state_index());
    return starting_state;
  }

  Eigen::Vector2d Gasedge::get_ending_state(
      Eigen::Ref<Eigen::VectorXd const> const &state) const {
    Eigen::Vector2d ending_state = state.segment<2>(get_ending_state_index());
    return ending_state;
  }
  Eigen::Vector2d Gasedge::get_boundary_state(
      Direction direction,
      Eigen::Ref<Eigen::VectorXd const> const &state) const {
    if (direction == start) {
      return get_starting_state(state);
    } else if (direction == end) {
      return get_ending_state(state);
    } else {
      auto *this_idobject = dynamic_cast<Network::Idobject const *>(this);
      if (!this_idobject) {
        gthrow(
            {"This gasedge is not and Idobject, which should never happen!"});
      }
      gthrow(
          {"The supplied direction was ", std::to_string(direction),
           ", which is not +1 or -1! Edge id is: ", this_idobject->get_id()});
    }
  }
} // namespace Model::Gas
