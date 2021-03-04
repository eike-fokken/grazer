#pragma once
#include "Valuecontainer.hpp"
#include <Eigen/Sparse>
#include <filesystem>
#include <memory>
#include <nlohmann/json.hpp>

namespace Aux {

  class Matrixhandler;
}

namespace Model::Networkproblem {

  /// \brief A base class for network components that supply model equations to be solved.
    
  class Equationcomponent {

  public:
    virtual ~Equationcomponent() {};

    /// \brief evaluates the model equations into rootvalues.
    ///
    /// @param[out] rootvalues Results of the model equations, when evaluated on the other parameters.
    /// @param last_time time point of the last time step. Usually important for
    /// PDEs
    /// @param new_time time point of the current time step.
    /// @param last_state value of the state at last time step.
    /// @param new_state value of the state at current time step.
    virtual void evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
                          double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
                          Eigen::Ref<Eigen::VectorXd const> const &new_state) const = 0;

    /// \brief derivative of Equationcomponent::evaluate.
    ///
    /// evaluates the derivative of Equationcomponent::evaluate and hands the
    /// result to jacobianhandler, which will fill the Jacobi matrix.
    /// @param jacobianhandler A helper object, that fills a sparse matrix in an
    /// efficient way.
    /// @param last_time time point of the last time step. Usually important for
    /// PDEs
    /// @param new_time time point of the current time step.
    /// @param last_state value of the state at last time step.
    /// @param new_state value of the state at current time step.
    virtual void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &, Eigen::Ref<Eigen::VectorXd const> const &new_state) const = 0;

    /// \brief Returns number of state variables needed by this component.
    ///
    /// Often this will be implemented by a function returning a literal
    /// int like 2. But for PDES its value is only known after construction.
    ///
    /// @returns number of state variables needed by this component

    virtual int get_number_of_states() const = 0;

    /// \brief Utility for setup of things that cannot be done during construction.
    ///
    /// is called during set_indices.
    /// Usually does nothing, but for example gas nodes
    /// claim indices from their attached gas edges.
    virtual void setup();


    /// \brief This function sets the indices #start_state_index and #after_state_index AND calls \code{cpp}setup()\endcode
    /// 
    /// @returns i The new lowest free index.
    int set_indices(int const next_free_index);

    /// \brief getter for #start_state_index
    int get_start_state_index() const;

    /// \brief getter for #after_state_index
    int get_after_state_index() const;


    /// \brief Prints values in #valuecontainer into a file in output_directory.
    hier weiter machen!
    virtual void
    print_to_files(std::filesystem::path const &output_directory) = 0;

    virtual void save_values(double time, Eigen::Ref<Eigen::VectorXd const> const &state) = 0;





    virtual void set_initial_values(Eigen::Ref<Eigen::VectorXd>new_state,
                                    nlohmann::ordered_json initial_json) = 0;

  protected:
    void push_to_values(double t, std::vector<std::map<double, double>>);
    void print_indices() const;

    void write_to_files();

    std::vector<double> const &get_times() const;
    std::vector<std::vector<std::map<double, double>>> const &
    get_values() const;

  private:
    std::unique_ptr<Valuecontainer> values_ptr{std::make_unique<Valuecontainer>()};

    int start_state_index{-1};
    int after_state_index{-1};
  };

} // namespace Model::Networkproblem
