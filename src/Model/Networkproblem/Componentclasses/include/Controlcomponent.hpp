#pragma once
#include "Equation_base.hpp"
#include "Timedata.hpp"
#include <Eigen/Dense>
#include <nlohmann/json.hpp>

namespace Aux {
  class Matrixhandler;
}
namespace Model {
  class SimpleControlcomponent;

  class Controlcomponent : public Equation_base {
    /** \brief SimpleControlcomponent is a friend of Controlcomponent to
     * give it access to #start_control_index and #after_control_index.
     */
    friend class SimpleControlcomponent;
    friend class Networkproblem;

  public:
    virtual ~Controlcomponent(){};

    /** \brief evaluates the model equations into rootvalues.
     *
     * @param[out] rootvalues Results of the model equations, when evaluated
     * on the other parameters.
     * @param last_time time point of the last time step. Usually important
     * for PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     * @param control value of the control at current time step.
     */
    virtual void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /** \brief Carries out steps that need to be taken before the Newton
     * method for a time step can start.
     *
     * For most components does nothing.
     * @param last_time time point of the last time step. Usually important
     * for PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     * @param control value of the control at current time step.
     */

    virtual void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control);

    /** \brief derivative of Controlcomponent::evaluate w.r.t. \p new_state.
     *
     * evaluates the derivative of Controlcomponent::evaluate and hands
     * the result to jacobianhandler, which will fill the Jacobi matrix.
     *
     * @param jacobianhandler A helper object, that fills a sparse matrix
     * in an efficient way.
     * @param last_time time point of the last time step. Usually important
     * for PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     * @param control value of the control at current time step.
     */
    virtual void d_evalutate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /** \brief derivative of Controlcomponent::evaluate w.r.t. \p
     * last_state.
     *
     * evaluates the derivative of Controlcomponent::evaluate and hands
     * the result to jacobianhandler, which will fill the Jacobi matrix.
     *
     * @param jacobianhandler A helper object, that fills a sparse matrix
     * in an efficient way.
     * @param last_time time point of the last time step. Usually important
     * for PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     * @param control value of the control at current time step.
     */
    virtual void d_evalutate_d_last_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /** \brief derivative of Controlcomponent::evaluate w.r.t. \p control.
     *
     * evaluates the derivative of Controlcomponent::evaluate and hands
     * the result to jacobianhandler, which will fill the Jacobi matrix.
     *
     * @param jacobianhandler A helper object, that fills a sparse matrix
     * in an efficient way.
     * @param last_time time point of the last time step. Usually important
     * for PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     * @param control value of the control at current time step.
     */
    virtual void d_evalutate_d_control(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /** \brief This function sets the indices #start_control_index and
     * #after_control_index.
     *
     * @param next_free_index The first control index that is currently not
     * claimed by another component.
     * @returns The new lowest free index.
     */
    virtual int set_control_indices(int next_free_index) = 0;

    int get_number_of_controls_per_timepoint() const;

    /** \brief getter for #start_control_index
     */
    int get_start_control_index() const;

    /** \brief getter for #after_control_index
     */
    int get_after_control_index() const;

    /** \brief Fills the indices owned by this component with control values
     */
    virtual void set_initial_controls(
        Timedata timedata, Eigen::Ref<Eigen::VectorXd> controls,
        nlohmann::json const &control_json)
        = 0;

    virtual void set_lower_bounds(
        Timedata timedata, Eigen::Ref<Eigen::VectorXd> lower_bounds,
        nlohmann::json const &lower_bound_json)
        = 0;

    virtual void set_upper_bounds(
        Timedata timedata, Eigen::Ref<Eigen::VectorXd> upper_bounds,
        nlohmann::json const &upper_bound_json)
        = 0;

    static std::optional<nlohmann::json> get_control_schema();

  private:
    /** \brief The first control index, this Controlcomponent "owns".
     */
    int start_control_index{-1};

    /** \brief The first control index after #start_control_index, that is
     * not "owned" by this Controlcomponent.
     */
    int after_control_index{-1};
  };
} // namespace Model
