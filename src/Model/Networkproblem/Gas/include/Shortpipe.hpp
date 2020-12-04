#include <Gasedge.hpp>

namespace Model::Networkproblem::Gas {

  class Shortpipe final: public Gasedge {

  public:
    using Gasedge::Gasedge;


    void evaluate(Eigen::VectorXd &rootfunction, double last_time,
                  double new_time, Eigen::VectorXd const &last_state,
                  Eigen::VectorXd const &new_state) const override;
    void evaluate_state_derivative( Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
                                    Eigen::VectorXd const &, Eigen::VectorXd const &new_state) const override;

    //maybe we should override display?
    //void display() const override;

    int get_number_of_states() const override;

    void print_to_files(std::filesystem::path const &output_directory) override;

    void save_values(double time, Eigen::VectorXd const &state) override;

    void set_initial_values(Eigen::VectorXd &new_state,
                            nlohmann::ordered_json initial_json) override;

    /// Shortpipes just use pressure and volumetric flow as their variables, so that this function simply returns the corresponding boundary state.
    Eigen::Vector2d get_boundary_p_qvol(int direction, Eigen::VectorXd const &state) const override;

    /// Because shortpipes use pressure and volumetric flow as their state variables, this function just hands `function_derivative` to jacobianhandler.
    void dboundary_p_qvol_dstate(int direction, Aux::Matrixhandler * jacobianhandler, Eigen::RowVector2d function_derivative, int rootvalues_index, Eigen::VectorXd const &state) const override;

  };

}
