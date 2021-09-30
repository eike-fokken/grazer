#pragma once
#include <Eigen/Dense>
#include <nlohmann/json.hpp>

namespace Aux {
  class Matrixhandler;
}

namespace Model::Networkproblem {
  class Controlcomponent {
  public:
    virtual ~Controlcomponent();

    /** \brief evaluates the model equations into rootvalues.
     *
     * @param[out] rootvalues Results of the model equations, when evaluated on
     * the other parameters.
     * @param last_time time point of the last time step. Usually important for
     * PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     */
    virtual void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &old_control,
        Eigen::Ref<Eigen::VectorXd const> const &new_control) const = 0;

    /** \brief Carries out steps that need to be taken before the Newton method
     * for a time step can start.
     *
     * For most components does nothing.
     * @param last_time time point of the last time step. Usually important for
     * PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     */

    virtual void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &old_control,
        Eigen::Ref<Eigen::VectorXd const> const &new_control);

    /** \brief derivative of Equationcomponent::evaluate.
     *
     * evaluates the derivative of Equationcomponent::evaluate and hands
     * the result to jacobianhandler, which will fill the Jacobi matrix.
     * @param jacobianhandler A helper object, that fills a sparse matrix
     * in an efficient way.
     * @param last_time time point of the last time step. Usually important
     * for PDEs
     * @param new_time time point of the current time step.
     * @param last_state value of the state at last time step.
     * @param new_state value of the state at current time step.
     */
    virtual void d_evalutate_d_new_state(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &old_control,
        Eigen::Ref<Eigen::VectorXd const> const &new_control) const = 0;

    /** \brief Utility for setup of things that cannot be done during
     * construction.
     *
     * is called by Networkproblems reserve_state_indices.
     * Usually does nothing, but for example gas nodes
     * claim indices from their attached gas edges.
     */
    virtual void setup();

    /** \brief Returns number of controls needed by this component per time
     *  step.
     *
     *  Often this will be implemented by a function returning a literal
     *  int like 1.
     *  @returns number of control variables needed by this component
     */
    virtual int get_number_of_controls() const = 0;

    /** \brief This function sets the indices #start_control_index and
     * #after_control_index.
     *
     * @param next_free_index The first control index that is currently not
     * claimed by another component.
     * @returns The new lowest free index.
     */
    int set_control_indices(int const next_free_index);

    /** \brief getter for #start_control_index
     */
    int get_start_control_index() const;

    /** \brief getter for #after_control_index
     */
    int get_after_control_index() const;

    /** \brief Fills the indices owned by this component with control values
     * from a json.
     *
     * Relies on the exact format of the json.
     * @param[out] new_state state vector, who shall contain the initial
     * values.
     * @param control_json Json object that contains the controls.
     */
    virtual void set_controls_of_timestep(
        double time, Eigen::Ref<Eigen::VectorXd> controls,
        nlohmann::json const &control_json)
        = 0;

  private:
    /** \brief The first control index, this Controlcomponent "owns".
     */
    int start_control_index{-1};

    /** \brief The first control index after #start_control_index, that is not
     * "owned" by this Equationcomponent.
     */
    int after_control_index{-1};
  };
} // namespace Model::Networkproblem
