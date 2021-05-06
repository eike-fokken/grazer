#pragma once
#include "Boundaryvalue.hpp"
#include "Node.hpp"
#include "Statecomponent.hpp"
#include <Eigen/Sparse>

namespace Model::Networkproblem::Power {

  class Powernode : public Statecomponent, public Network::Node {

  public:
    virtual std::string get_power_type() const = 0;
    static nlohmann::json get_schema();

    Powernode(nlohmann::json const &topology);

    ~Powernode() override{};

    void setup() override;

    int get_number_of_states() const final;

    double get_G() const;
    double get_B() const;

    void print_to_files(nlohmann::json &new_output) override;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json) final;

    double P(Eigen::Ref<Eigen::VectorXd const> state) const;
    double Q(Eigen::Ref<Eigen::VectorXd const> state) const;
    void evaluate_P_derivative(
        int equationindex, Aux::Matrixhandler *jacobianhandler,
        Eigen::Ref<Eigen::VectorXd const> new_state) const;
    void evaluate_Q_derivative(
        int equationindex, Aux::Matrixhandler *jacobianhandler,
        Eigen::Ref<Eigen::VectorXd const> new_state) const;

  protected:
    void json_save_power(
        double time, Eigen::Ref<Eigen::VectorXd const> state, double P_val,
        double Q_val);

    Boundaryvalue<2> const boundaryvalue;
    /// Real part of the admittance of this node
    double G;
    /// Imaginary part of the admittance of this node
    double B;

  private:
    /// \brief number of state variables, this component needs.
    static constexpr int number_of_state_variables{2};

    std::vector<std::tuple<double, double, Powernode *>>
        attached_component_data;
  };

} // namespace Model::Networkproblem::Power
