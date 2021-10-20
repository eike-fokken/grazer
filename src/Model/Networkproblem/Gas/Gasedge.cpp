#include "Gasedge.hpp"
#include "Exception.hpp"
#include "Idobject.hpp"
#include "make_schema.hpp"

namespace Model::Gas {

  int Gasedge::init_vals_per_interpol_point() { return 2; }

  nlohmann::json Gasedge::get_initial_schema() {
    int interpol_points = 2;
    std::vector<nlohmann::json> contains_x
        = {R"({"minimum": 0, "maximum": 0})"_json,
           R"({"minimum": 1, "maximum": 1})"_json};
    return Aux::schema::make_initial_schema(
        interpol_points, Gasedge::init_vals_per_interpol_point(), contains_x);
  }

  int Gasedge::give_away_start_index() const {
    if (get_state_startindex() < 0 or get_state_afterindex() < 0) {
      gthrow(
          {"This function: ", __FUNCTION__,
           " can only be called after set_state_indices(...) has been "
           "called."});
    }
    return get_state_startindex();
  }

  int Gasedge::give_away_end_index() const {
    if (get_state_startindex() < 0 or get_state_afterindex() < 0) {
      gthrow(
          {"This function: ", __FUNCTION__,
           " can only be called after set_state_indices(...) has been "
           "called."});
    }
    return (get_state_afterindex() - 1);
  }
  int Gasedge::boundary_equation_index(int direction) const {
    if (direction == 1) {
      return give_away_start_index();
    } else if (direction == -1) {
      return give_away_end_index();
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

  int Gasedge::get_equation_start_index() const {
    return get_starting_state_index() + 1; // Nofstates/2;
  }
  int Gasedge::get_equation_after_index() const {
    return get_state_afterindex() - 1; // - Nofstates / 2 + 1 ;
  }

  int Gasedge::get_starting_state_index() const {
    return get_state_startindex();
  }

  int Gasedge::get_ending_state_index() const {

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

  int Gasedge::get_boundary_state_index(int direction) const {
    if (direction == 1) {
      return get_starting_state_index();
    } else if (direction == -1) {
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
      int direction, Eigen::Ref<Eigen::VectorXd const> const &state) const {
    if (direction == 1) {
      return get_starting_state(state);
    } else if (direction == -1) {
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
