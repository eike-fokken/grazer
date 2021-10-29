#pragma once
#include "Equationcomponent.hpp"
#include "SimpleStatecomponent.hpp"
#include <Eigen/Sparse>

namespace Model::Gas {

  /**
   * @brief This class is a base class for all Gas edges with a 2x2 balance law
   *
   */
  class Gasedge : public SimpleStatecomponent {

  public:
    static int init_vals_per_interpol_point();

    virtual std::string get_gas_type() const = 0;
    Eigen::Index boundary_equation_index(int direction) const;

    Eigen::Index get_boundary_state_index(int direction) const;

    Eigen::Index get_equation_start_index() const;
    Eigen::Index get_equation_after_index() const;

    /**
     * @brief Returns the boundary
     *
     * @param    direction            TODO
     * @param    state                TODO
     * @return Eigen::Vector2d
     */
    Eigen::Vector2d get_boundary_state(
        int direction, Eigen::Ref<Eigen::VectorXd const> const &state) const;

    /**
     * @brief returns the boundary state expressed in pressure and volumetric
     * flow. It is the responsibility of the edge to decide what that means.
     *
     * @param    direction            TODO
     * @param    state                TODO
     * @return Eigen::Vector2d
     */
    virtual Eigen::Vector2d get_boundary_p_qvol_bar(
        int direction,
        Eigen::Ref<Eigen::VectorXd const> const &state) const = 0;

    /**
     * @brief Set the derivatives with respect to the boundary states.
     *
     * If (p,qvol) = phi(rho,q), then this function must compute
     * \code{.cpp}dF/dState = function_derivative*phi'\endcode and call
     * \code{.cpp}
     * jacobianhandler.set_coefficient(rootvalues_index, rho_index,
     * dF/dState[0]); jacobianhandler.set_coefficient(rootvalues_index,
     * q_index, dF/dState[1]); \endcode
     *
     * @param    direction            TODO
     * @param    jacobianhandler      TODO
     * @param    function_derivative  TODO
     * @param    rootvalues_index     TODO
     * @param    state                TODO
     */
    virtual void dboundary_p_qvol_dstate(
        int direction, Aux::Matrixhandler &jacobianhandler,
        Eigen::RowVector2d function_derivative, Eigen::Index rootvalues_index,
        Eigen::Ref<Eigen::VectorXd const> const &state) const = 0;

  private:
    Eigen::Vector2d
    get_starting_state(Eigen::Ref<Eigen::VectorXd const> const &state) const;
    Eigen::Vector2d
    get_ending_state(Eigen::Ref<Eigen::VectorXd const> const &state) const;

    Eigen::Index give_away_start_index() const;
    Eigen::Index give_away_end_index() const;

    Eigen::Index get_starting_state_index() const;
    Eigen::Index get_ending_state_index() const;
  };

} // namespace Model::Gas
