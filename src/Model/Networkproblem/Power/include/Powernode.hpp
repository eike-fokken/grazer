#pragma once
#include "Equationcomponent.hpp"
#include "Indexmanager.hpp"
#include "InterpolatingVector.hpp"
#include "Node.hpp"
#include "Statecomponent.hpp"
#include <Eigen/Sparse>

namespace Model::Power {

  class Powernode :
      public Equationcomponent,
      public Statecomponent,
      public Network::Node {

  public:
    virtual std::string get_power_type() const = 0;
    static nlohmann::json get_schema();
    static std::optional<nlohmann::json> get_boundary_schema();
    static nlohmann::json get_initial_schema();
    static int init_vals_per_interpol_point();

    Powernode(nlohmann::json const &topology);
    ~Powernode() override{};

    void setup() override;

    double get_G() const;
    double get_B() const;

    void add_results_to_json(nlohmann::json &new_output) override;

    int get_number_of_states() const override;
    int get_start_state_index() const override;
    int get_after_state_index() const override;

    int set_state_indices(int next_free_index) final;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json) final;

    double P(Eigen::Ref<Eigen::VectorXd const> const &state) const;
    double Q(Eigen::Ref<Eigen::VectorXd const> const &state) const;
    void evaluate_P_derivative(
        int equationindex, Aux::Matrixhandler &jacobianhandler,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const;
    void evaluate_Q_derivative(
        int equationindex, Aux::Matrixhandler &jacobianhandler,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const;

  protected:
    void json_save_power(
        double time, Eigen::Ref<Eigen::VectorXd const> const &state,
        double P_val, double Q_val);

    Aux::InterpolatingVector const boundaryvalue;
    /// Real part of the admittance of this node
    double G;
    /// Imaginary part of the admittance of this node
    double B;

  private:
    Aux::Timeless_Indexmanager statemanager;
    /// \brief number of state variables, this component needs.
    static constexpr int number_of_state_variables{2};

    std::vector<std::tuple<double, double, Powernode *>>
        attached_component_data;
  };

} // namespace Model::Power
