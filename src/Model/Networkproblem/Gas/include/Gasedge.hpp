#pragma once
#include <Edge.hpp>
#include <Eigen/Sparse>
#include <Equationcomponent.hpp>

namespace Model::Networkproblem::Gas {

  /// @brief This class is a base class for all Gas edges with a 2x2 balance law
  class Gasedge : public Network::Edge, public Equationcomponent {

  public:
    using Edge::Edge;

    int give_away_start_index();
    int give_away_end_index();
    int give_away_boundary_index(int direction);

    int get_equation_start_index() const;

    int get_equation_after_index() const;

    int get_starting_state_index() const;
    int get_ending_state_index() const;
    int get_boundary_state_index(int direction) const;

    Eigen::Vector2d get_starting_state(Eigen::VectorXd const &state) const;
    Eigen::Vector2d get_ending_state(Eigen::VectorXd const &state) const;
    Eigen::Vector2d get_boundary_state(int direction, Eigen::VectorXd const &state) const;
  };

} // namespace Model::Networkproblem::Gas
