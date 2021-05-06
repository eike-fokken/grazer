#pragma once
#include <Eigen/Sparse>
#include <filesystem>
#include <memory>
#include <nlohmann/json.hpp>

namespace Aux {

  class Matrixhandler;
}

namespace Model::Networkproblem {

  /// \brief A base class for network components that supply model equations to
  /// be solved.
  ///
  /// An equation component owns a number of indices, namely between
  /// #start_state_index (inclusive) and #after_state_index (exclusive), which
  /// will be filled with results of the model equations of the component.
  /// It is implicitely taken for granted that the set of owned indices of all
  /// objects starts at 0 and is consecutive.
  class Equationcomponent {

  public:
    virtual ~Equationcomponent(){};

    /// \brief evaluates the model equations into rootvalues.
    ///
    /// @param[out] rootvalues Results of the model equations, when evaluated on
    /// the other parameters.
    /// @param last_time time point of the last time step. Usually important for
    /// PDEs
    /// @param new_time time point of the current time step.
    /// @param last_state value of the state at last time step.
    /// @param new_state value of the state at current time step.
    virtual void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state) const = 0;

    /// \brief Carries out steps that need to be taken before the Newton method
    /// for a time step can start.
    ///
    /// For most components does nothing.
    /// @param last_time time point of the last time step. Usually important for
    /// PDEs
    /// @param new_time time point of the current time step.
    /// @param last_state value of the state at last time step.
    /// @param new_state value of the state at current time step.
    virtual void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state);

    /// \brief derivative of Equationcomponent::evaluate.
    ///
    /// evaluates the derivative of Equationcomponent::evaluate and hands
    /// the result to jacobianhandler, which will fill the Jacobi matrix.
    /// @param jacobianhandler A helper object, that fills a sparse matrix
    /// in an efficient way.
    /// @param last_time time point of the last time step. Usually important
    /// for PDEs
    /// @param new_time time point of the current time step.
    /// @param last_state value of the state at last time step.
    /// @param new_state value of the state at current time step.
    virtual void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const>,
        Eigen::Ref<Eigen::VectorXd const> new_state) const = 0;

    /// \brief Utility for setup of things that cannot be done during
    /// construction.
    ///
    /// is called during set_indices.
    /// Usually does nothing, but for example gas nodes
    /// claim indices from their attached gas edges.
    virtual void setup();

    static std::optional<nlohmann::json> get_boundary_schema();
  };

} // namespace Model::Networkproblem
