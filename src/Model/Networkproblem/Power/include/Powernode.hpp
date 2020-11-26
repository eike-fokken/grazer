#pragma once
#include <Boundaryvalue.hpp>
#include <Eigen/Sparse>
#include <Equationcomponent.hpp>
#include <Node.hpp>
#include <nlohmann/json.hpp>

namespace Model::Networkproblem::Power {

  class Powernode : public Equationcomponent, public Network::Node {

  public:
    Powernode(std::string _id, nlohmann::ordered_json boundary_json, double _G,
              double _B);

    virtual ~Powernode(){};

    int get_number_of_states() const final;

    // int get_number_of_printout_states() const final;

    double get_G() const;
    double get_B() const;

    void print_to_files(std::filesystem::path const &output_directory) final;

    void set_initial_values(Eigen::VectorXd &new_state,
                            nlohmann::ordered_json initial_json) final;

  protected:
    void save_values(double time, Eigen::VectorXd const &state) final;
    double P(Eigen::VectorXd const &new_state) const;
    double Q(Eigen::VectorXd const &new_state) const;
    void evaluate_P_derivative(Aux::Matrixhandler *jacobianhandler,
                               Eigen::VectorXd const &new_state) const;
    void evaluate_Q_derivative(Aux::Matrixhandler *jacobianhandler,
                               Eigen::VectorXd const &new_state) const;

    Boundaryvalue<Powernode, 2> boundaryvalue;
    /// Real part of the admittance of this node
    double G;
    /// Imaginary part of the admittance of this node
    double B;

  };

} // namespace Model::Networkproblem::Power
