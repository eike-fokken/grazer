#pragma once
#include "Edge.hpp"
#include "Gasedge.hpp"

namespace Model::Networkproblem::Gas {

  class Shortpipe: public Network::Edge, public Gasedge {

  public:
    static std::string get_type();
    static bool needs_boundary_values();


    using Edge::Edge;

    void evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
                  double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
                  Eigen::Ref<Eigen::VectorXd const> const &new_state) const override final;
    void evaluate_state_derivative( Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
                                    Eigen::Ref<Eigen::VectorXd const> const &, Eigen::Ref<Eigen::VectorXd const> const &new_state) const override final;

    //maybe we should override display?
    //void display() const override;

    int get_number_of_states() const override final;

    void print_to_files(std::filesystem::path const &output_directory) override final;

    void save_values(double time, Eigen::Ref<Eigen::VectorXd const> const &state) override final;

    void set_initial_values(Eigen::Ref<Eigen::VectorXd>new_state,
                            nlohmann::ordered_json initial_json) override final;

    /// Shortpipes just use pressure and volumetric flow as their variables, so that this function simply returns the corresponding boundary state.
    Eigen::Vector2d get_boundary_p_qvol_bar(int direction, Eigen::Ref<Eigen::VectorXd const> const &state) const override final;

    /// Because shortpipes use pressure and volumetric flow as their state variables, this function just hands `function_derivative` to jacobianhandler.
    void dboundary_p_qvol_dstate(int direction, Aux::Matrixhandler * jacobianhandler, Eigen::RowVector2d function_derivative, int rootvalues_index, Eigen::Ref<Eigen::VectorXd const> const &state) const override final;

  };

}
