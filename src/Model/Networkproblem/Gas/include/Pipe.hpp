#pragma once
#include "Edge.hpp"
#include <Gasedge.hpp>
#include <Isothermaleulerequation.hpp>
#include <Implicitboxscheme.hpp>

namespace Model::Networkproblem::Gas {

  class Pipe final: public Gasedge, public Network::Edge {
  public:


    static std::string get_type();

    Pipe(nlohmann::json const &topology, std::vector<std::unique_ptr<Network::Node>> &nodes);

    void evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
                  double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
                  Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;
    void evaluate_state_derivative(
                                   Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
                                   Eigen::Ref<Eigen::VectorXd const> const &, Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;



    void display() const override;

    int get_number_of_states() const override;

    void
    print_to_files(std::filesystem::path const &output_directory) override;

    void save_values(double time, Eigen::Ref<Eigen::VectorXd const> const &state) override;

    void set_initial_values(Eigen::Ref<Eigen::VectorXd>new_state,
                            nlohmann::ordered_json initial_json) override;


    Eigen::Vector2d get_boundary_p_qvol_bar(int direction, Eigen::Ref<Eigen::VectorXd const> const &state) const override;


    void dboundary_p_qvol_dstate(int direction, Aux::Matrixhandler * jacobianhandler, Eigen::RowVector2d function_derivative, int rootvalues_index, Eigen::Ref<Eigen::VectorXd const> const &state) const override;





    double const length;
    double const diameter;
    double const roughness;
    int const number_of_points;
    double const Delta_x;

    Balancelaw::Isothermaleulerequation const bl;
    Model::Scheme::Implicitboxscheme const scheme;



  };


}
