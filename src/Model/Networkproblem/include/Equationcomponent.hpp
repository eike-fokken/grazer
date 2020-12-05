#pragma once
#include <Eigen/Sparse>
#include <filesystem>
#include <nlohmann/json.hpp>

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

    virtual void evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
                          double new_time, Eigen::VectorXd const &last_state,
                          Eigen::VectorXd const &new_state) const = 0;
    virtual void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::VectorXd const &, Eigen::VectorXd const &new_state) const = 0;



    /// Returns number of state variables needed by this component.
    /// Usually this will be implemented by a function returning a literal
    /// int like 2.
        virtual int get_number_of_states() const = 0;

    /// is called during set_indices to do something.
    /// Usually does nothing, but for example gas nodes
    /// claim indices from their attached gas edges.
    virtual void setup();


    // /// Determines the number of state variables to be printed into output
    // /// files.  Defaults to get_number_of_states() but can be overridden.
    // /// @returns Number of states to be printed out.
    // virtual int get_number_of_printout_states() const;

    /// This function sets the index.
    /// @returns the new lowest free index.
    int set_indices(int const next_free_index);

    int get_start_state_index() const;
    int get_after_state_index() const;

    void print_out_files();

    virtual void
    print_to_files(std::filesystem::path const &output_directory) = 0;

    virtual void save_values(double time, Eigen::VectorXd const &state) = 0;





    virtual void set_initial_values(Eigen::VectorXd &new_state,
                                    nlohmann::ordered_json initial_json) = 0;

  protected:
    void push_to_values(double t, std::vector<std::map<double, double>>);
    void print_indices() const;

    void write_to_files();

    std::vector<double> const &get_times() const;
    std::vector<std::vector<std::map<double, double>>> const &
    get_values() const;

  private:
    /// This must be refactored into a container of times and values.
    std::vector<double> eqtimes;
    std::vector<std::vector<std::map<double, double>>> eqvalues;
    int start_state_index{-1};
    int after_state_index{-1};
  };

} // namespace Model::Networkproblem
