#pragma once
#include <Boundaryvalue.hpp>
#include <Equationcomponent.hpp>
#include <string>

namespace Model::Networkproblem::Power {

  class Powernode : public Equationcomponent, public Network::Node {

  public:
    Powernode(std::string _id,
              std::map<double, Eigen::Matrix<double, 2, 1>> _boundary_values,
              double _G, double _B)
        : Node(_id), boundaryvalue(_boundary_values), G(_G), B(_B){};

    virtual ~Powernode(){};

    int get_number_of_states() const final;

    double get_G() const;
    double get_B() const;

  protected:
    void push_values(double time, Eigen::VectorXd const &state) override;
    double P(double new_time, Eigen::VectorXd const &new_state);
    double Q(double new_time, Eigen::VectorXd const &new_state);
    void evaluate_P_derivative(Aux::Matrixhandler *jacobianhandler,
                               Eigen::VectorXd const &new_state);
    void evaluate_Q_derivative(Aux::Matrixhandler *jacobianhandler,
                               Eigen::VectorXd const &new_state);

    Boundaryvalue<Powernode, 2> boundaryvalue;
    /// Real part of the admittance of this node
    double G;
    /// Imaginary part of the admittance of this node
    double B;
  };

} // namespace Model::Networkproblem::Power
