#pragma once
#include "Eigen/Sparse"

namespace Aux {

  class Matrixhandler;
}

namespace Model::Networkproblem {
  class Inequalitycomponent {

  public:
    virtual ~Inequalitycomponent(){};

    virtual void evaluate_inequality(
        Eigen::Ref<Eigen::VectorXd> inequality_values, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const = 0;

    virtual void d_evaluate_inequality_d_new_state(
        Aux::Matrixhandler &inequality_new_state_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const = 0;

    virtual void d_evaluate_inequality_d_old_state(
        Aux::Matrixhandler &inequality_old_state_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const = 0;

    virtual void d_evaluate_inequality_d_control(
        Aux::Matrixhandler &inequality_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const = 0;

    virtual void d_evaluate_inequality_d_last_control(
        Aux::Matrixhandler &inequality_old_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const = 0;

    /** \brief Returns number of inequalities set by this component.
     *
     * Often this will be implemented by a function returning a literal
     * int like 2.  But it may be only known after construction, therefore this
     * is not done in the constructor.
     *
     * @returns number of inequalities set by this component
     */
    virtual int get_number_of_inequalities() const = 0;

    /** \brief This function sets the indices #start_inequality_index and
     * #after_inequality_index.
     *
     * @param next_free_index The first inequality index that is currently not
     * claimed by another component.
     * @returns The new lowest free index.
     */
    int set_inequality_indices(int const next_free_index);

    /** \brief getter for #start_inequality_index
     */
    int get_start_inequality_index() const;

    /** \brief getter for #after_inequality_index
     */
    int get_after_inequality_index() const;

  private:
    /** \brief The first index, this Inequalitycomponent "owns".
     */
    int start_inequality_index{-1};

    /** \brief The first index after #start_inequality_index, that is not
     * "owned" by this Inequalitycomponent.
     */
    int after_inequality_index{-1};
  };
} // namespace Model::Networkproblem
