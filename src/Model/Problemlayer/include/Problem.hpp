#pragma once
#include <Eigen/Sparse>
#include <Subproblem.hpp>
#include <memory>
#include <vector>

namespace Aux {

  class Matrixhandler;
}

/*! \namespace Model
 *  This namespace holds all data for setting up model equations and for taking
 * derivatives thereof.
 */
namespace Model {

  class Problem {

  public:
    /// The constructor needs to declare Delta_t
    ///
    Problem(double delta_t) : Delta_t(delta_t){};

    void add_subproblem(std::unique_ptr<Subproblem> subproblem_ptr);

    unsigned int set_indices();

    void evaluate(Eigen::VectorXd &rootfunction, double last_time,
                  double new_time, Eigen::VectorXd const &last_state,
                  Eigen::VectorXd const &new_state);

    void evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                   double last_time, double new_time,
                                   Eigen::VectorXd const &last_state,
                                   Eigen::VectorXd const &new_state);

    /// As we have unique pointers, we can only give back a pointer to our
    /// subproblems.
    std::vector<Subproblem *> get_subproblems();

  private:
    /// collection of sub-problems
    std::vector<std::unique_ptr<Subproblem>> subproblems;

  protected:
    /// The time stepsize, which is immutable.
    const double Delta_t;
  };

} // namespace Model
