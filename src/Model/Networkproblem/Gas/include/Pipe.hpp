#pragma once
#include <Gasedge.hpp>
#include <Isothermaleulerequation.hpp>
#include <Implicitboxscheme.hpp>

namespace Model::Networkproblem::Gas {

  class Pipe final: public Gasedge {
  public:

    Pipe(std::string _id, Network::Node *start_node, Network::Node *end_node, nlohmann::ordered_json topology_json, double Delta_x);



    void evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
                  double new_time, Eigen::VectorXd const &last_state,
                  Eigen::VectorXd const &new_state) const override;
    void evaluate_state_derivative(
                                   Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
                                   Eigen::VectorXd const &, Eigen::VectorXd const &new_state) const override;

    
    int get_number_of_states() const override;

    void
    print_to_files(std::filesystem::path const &output_directory) override;

    void save_values(double time, Eigen::VectorXd const &state) override;

    void set_initial_values(Eigen::VectorXd &new_state,
                            nlohmann::ordered_json initial_json) override;


    Eigen::Vector2d get_boundary_p_qvol(int direction, Eigen::VectorXd const &state) const override;


    void dboundary_p_qvol_dstate(int direction, Aux::Matrixhandler * jacobianhandler, Eigen::RowVector2d function_derivative, int rootvalues_index, Eigen::VectorXd const &state) const override;




  private:
    double const length;
    double const diameter;
    double const roughness;
    int const number_of_points;
    double const Delta_x;

    Balancelaw::Isothermaleulerequation bl;
    Model::Scheme::Implicitboxscheme scheme;



  };


}
