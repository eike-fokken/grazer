#pragma once
#include <Edge.hpp>
#include <Eigen/Sparse>
#include <Node.hpp>

namespace Model::Networkproblem {

/// This class defines all network components, that supply model equations.
/// Inheriting classes must especially override the private get_number_of_states().
class Equationcomponent {

public:
  virtual ~Equationcomponent(){};

  virtual void evaluate(Eigen::VectorXd & rootfunction, double current_time, double next_time, const Eigen::VectorXd &current_state,
                        Eigen::VectorXd &new_state) = 0;
  virtual void evaluate_state_derivative(double current_time, double next_time, const Eigen::VectorXd &,
                                         Eigen::SparseMatrix<double> &) = 0;

  /// Returns number of state variables needed by this component.
  /// Usually this will be implemented by a function returning a literal
  /// unsigned int like 2.
  virtual unsigned int get_number_of_states() = 0;

  unsigned int set_indices(unsigned int const next_free_index);

  unsigned int get_start_state_index() const;
  unsigned int get_after_state_index() const;

private:
  unsigned int start_state_index;
  unsigned int after_state_index;


};

/// This is an interface class, that defines objects that are nodes and have
/// equations.
class Equationnode : public Equationcomponent, public Network::Node {
public:
  virtual ~Equationnode(){};
};
/// This is an interface class, that defines objects that are edges and have
/// equations.
class Equationedge : public Equationcomponent, public Network::Edge {
public:
  virtual ~Equationedge(){};
};

} // namespace Model::Networkproblem
