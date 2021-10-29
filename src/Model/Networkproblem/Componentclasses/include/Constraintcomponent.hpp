#pragma once
#include "Eigen/Sparse"
#include "Timedata.hpp"
#include <nlohmann/json.hpp>

namespace Aux {
  class InterpolatingVector;
  class Matrixhandler;
} // namespace Aux

namespace Model {
  class SimpleConstraintcomponent;

  class Constraintcomponent {
    /** \brief SimpleConstraintcomponent is a friend of Constraintcomponent to
     * give it access to #start_constraint_index and #after_constraint_index.
     */
    friend class SimpleConstraintcomponent;
    friend class Networkproblem;

  public:
    static nlohmann::json get_constraint_schema() = delete;

    virtual ~Constraintcomponent(){};

    /**
     * @brief evaluates the model constraint equations into constraint_values
     *
     * @param[out]  constraint_values    results of the model constraint
     * equations.
     * @param       last_time            time point of the last time step.
     * Usually important for PDEs.
     * @param       new_time             time point of the current time step.
     * @param       state                value of the state at the current time
     * step.
     * @param       control              value of the control at the current
     * time step.
     */
    virtual void evaluate_constraint(
        Eigen::Ref<Eigen::VectorXd> constraint_values, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /**
     * @brief derivative of Constraintcomponent::evaluate_constraint w.r.t. \p
     * new state.
     *
     * @param    constraint_new_state_jacobian_handler  A helper object, that
     * fills a sparse matrix in an efficient way.
     * @param    last_time            time point of the last time step. Usually
     * important for PDEs.
     * @param    new_time             time point of the current time step.
     * @param    state                value of the state at the current time
     * step.
     * @param    control              value of the control at the current time
     * step.
     */
    virtual void d_evaluate_constraint_d_state(
        Aux::Matrixhandler &constraint_new_state_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /**
     * @brief derivative of Constraintcomponent::evaluate w.r.t. \p last_state.
     *
     * @param    constraint_control_jacobian_handler  A hlper object, that fills
     * a sparse matrix in an efficient way.
     * @param    last_time            time point of the last time step. Usually
     * important for PDEs.
     * @param    new_time             time point of the current time step.
     * @param    state                value of the state at current time step.
     * @param    control              value of the control at current time step.
     */
    virtual void d_evaluate_constraint_d_control(
        Aux::Matrixhandler &constraint_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /** \brief This function sets the indices #constraint_startindex and
     * #constraint_afterindex.
     *
     * @param next_free_index The first constraint index that is currently
     * not claimed by another component.
     * @returns The new lowest free index.
     */
    virtual Eigen::Index
    set_constraint_indices(Eigen::Index const next_free_index)
        = 0;

    /** @brief returns #constraint_afterindex - #constraint_startindex.
     */
    Eigen::Index get_number_of_constraints_per_timepoint() const;

    /** \brief getter for #start_constraint_index
     */
    Eigen::Index get_constraint_startindex() const;

    /** \brief getter for #after_constraint_index
     */
    Eigen::Index get_constraint_afterindex() const;

    virtual void set_constraint_lower_bounds(
        Aux::InterpolatingVector &full_control_vector,
        nlohmann::json const &constraint_lower_bounds_json) const = 0;

    virtual void set_constraint_upper_bounds(
        Aux::InterpolatingVector &full_control_vector,
        nlohmann::json const &constraint_upper_bounds_json) const = 0;

  private:
    /** \brief The first index, this Constraintcomponent "owns".
     */
    Eigen::Index constraint_startindex{-1};

    /** \brief The first index after #start_constraint_index, that is not
     * "owned" by this Constraintcomponent.
     */
    Eigen::Index constraint_afterindex{-1};
  };
} // namespace Model
