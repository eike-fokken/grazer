#pragma once
#include <Edge.hpp>
#include <Eigen/Sparse>
#include <Equationcomponent.hpp>

namespace Model::Networkproblem::Gas {

  /// @brief This class is a base class for all Gas edges with a 2x2 balance law
  class Gasedge : public Network::Edge, public Equationcomponent {

  public:
    using Edge::Edge;

    int give_away_boundary_index(int direction) const;

    int get_boundary_state_index(int direction) const;

    int get_equation_start_index() const;
    int get_equation_after_index() const;


    /// Returns the boundary
    Eigen::Vector2d get_boundary_state(int direction, Eigen::VectorXd const &state) const;

    /// returns the boundary state expressed in pressure and volumetric flow.
    /// It is the responsibility of the edge to decide what that means.
    virtual Eigen::Vector2d get_boundary_p_qvol(int direction, Eigen::VectorXd const &state) const = 0;


    /// @brief Set the derivatives with respect to the boundary states.
    ///
    /// If (p,qvol) = phi(rho,q), then this function must compute \code{.cpp}dF/dState = function_derivative*phi'\endcode and call
    /// \code{.cpp}
    /// jacobianhandler->set_coefficient(rootvalues_index, rho_index, dF/dState[0]);
    /// jacobianhandler->set_coefficient(rootvalues_index, q_index, dF/dState[1]);
    /// \endcode
    virtual void dboundary_p_qvol_dstate(int direction, Aux::Matrixhandler * jacobianhandler, Eigen::RowVector2d function_derivative, int rootvalues_index, Eigen::VectorXd const &state) const = 0;

  private:

    Eigen::Vector2d get_starting_state(Eigen::VectorXd const &state) const;
    Eigen::Vector2d get_ending_state(Eigen::VectorXd const &state) const;

    int give_away_start_index() const;
    int give_away_end_index() const;

    int get_starting_state_index() const;
    int get_ending_state_index() const;



  };

} // namespace Model::Networkproblem::Gas
