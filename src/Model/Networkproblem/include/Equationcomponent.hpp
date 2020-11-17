#pragma once
#include <Edge.hpp>
#include <Eigen/Sparse>
#include <Node.hpp>

namespace Aux {

  class Matrixhandler;
}

namespace Model::Networkproblem {

  /// This class defines all network components, that supply model equations.
  /// Inheriting classes must especially override the private
  /// get_number_of_states().
  class Equationcomponent {

  public:
    virtual ~Equationcomponent(){};

    virtual void evaluate(Eigen::VectorXd &rootfunction, double last_time,
                          double new_time, Eigen::VectorXd const &last_state,
                          Eigen::VectorXd const &new_state) = 0;
    virtual void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::VectorXd const &, Eigen::VectorXd const &new_state) = 0;

    /// Returns number of state variables needed by this component.
    /// Usually this will be implemented by a function returning a literal
    /// int like 2.
    virtual int get_number_of_states() const = 0;

    int set_indices(int const next_free_index);

    int get_start_state_index() const;
    int get_after_state_index() const;

  protected:
    void print_indices();

  private:
    int start_state_index{};
    int after_state_index{};
  };

} // namespace Model::Networkproblem
