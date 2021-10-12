#pragma once
#include "Eigen/Sparse"

namespace Aux {

  class Matrixhandler;
}

namespace Model::Networkproblem {
  class SimpleInequalitycomponent;

  class Inequalitycomponent {
    /** \brief SimpleInequalitycomponent is a friend of Inequalitycomponent to
     * give it access to #start_inequality_index and #after_inequality_index.
     */
    friend class SimpleInequalitycomponent;
    friend class Networkproblem;

  public:
    virtual ~Inequalitycomponent(){};

    virtual void evaluate_inequality(
        Eigen::Ref<Eigen::VectorXd> inequality_values, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    virtual void d_evaluate_inequality_d_state(
        Aux::Matrixhandler &inequality_new_state_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    virtual void d_evaluate_inequality_d_control(
        Aux::Matrixhandler &inequality_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /** \brief This function sets the indices #start_inequality_index and
     * #after_inequality_index.
     *
     * @param next_free_index The first inequality index that is currently
     * not claimed by another component.
     * @returns The new lowest free index.
     */
    virtual int set_inequality_indices(int const next_free_index) = 0;

    int get_number_of_inequalities_per_timepoint() const;

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
