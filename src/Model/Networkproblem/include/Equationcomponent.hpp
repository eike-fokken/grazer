#pragma once
#include <Edge.hpp>
#include <Eigen/Sparse>
#include <Node.hpp>
#include <map>
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

    /// This function sets the index.
    /// @returns the new lowest free index.
    int set_indices(int const next_free_index);

    int get_start_state_index() const;
    int get_after_state_index() const;

    virtual void push_values(double time, Eigen::VectorXd const &state) = 0;

    void print_out_files();

  protected:
    void push_to_values(double t, std::vector<std::map<double, double>>);
    void print_indices() const;

    void write_to_files();

  private:
    /// This must be refactored into a container of times and values.
    std::vector<double> times;
    std::vector<std::vector<std::map<double, double>>> values;
    int start_state_index{-1};
    int after_state_index{-1};
  };

} // namespace Model::Networkproblem
