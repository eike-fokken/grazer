#pragma once
#include "Boundaryvalue.hpp"
#include "Control.hpp"
#include "Edge.hpp"
#include <Gasedge.hpp>

namespace Model::Networkproblem::Power {
  class Powernode;
} // namespace Model::Networkproblem::Power

namespace Model::Networkproblem::Gaspowerconnection {

  /** \brief an edge connecting a gasnode to a powernode and modelling a
   * gaspower plant/power-to-gas plant.
   *
   * The plant can be used in  gas-controlled mode and power-controlled mode.
   * In gas-controlled mode, it uses its boundary values as a set pressure to be
   * maintained at its inlet and converts all resulting flow into power.
   *
   * In power-controlled mode, it doesn't enforce the pressure. In this case an
   * additional equation must be set in the powernode at the connections end.
   */
  class Gaspowerconnection final : public Gas::Gasedge, public Network::Edge {
  public:
    static std::string get_type();
    std::string get_gas_type() const override;
    static nlohmann::json get_schema();

    Gaspowerconnection(
        nlohmann::json const &topology,
        std::vector<std::unique_ptr<Network::Node>> &nodes);

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state) const override;
    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const>,
        Eigen::Ref<Eigen::VectorXd const> new_state) const override;

    void setup() override;

    int get_number_of_states() const override;

    void print_to_files(std::filesystem::path const &output_directory) override;

    void new_print_to_files(nlohmann::json &new_output) override;

    void
    save_values(double time, Eigen::Ref<Eigen::VectorXd const> state) override;

    void
    json_save(double time, Eigen::Ref<const Eigen::VectorXd> state) override;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json) override;

    Eigen::Vector2d get_boundary_p_qvol_bar(
        int direction, Eigen::Ref<Eigen::VectorXd const> state) const override;

    void dboundary_p_qvol_dstate(
        int direction, Aux::Matrixhandler *jacobianhandler,
        Eigen::RowVector2d function_derivative, int rootvalues_index,
        Eigen::Ref<Eigen::VectorXd const> state) const override;

    double smoothing_polynomial(double q) const;
    double dsmoothing_polynomial_dq(double q) const;

    double generated_power(double q) const;
    double dgenerated_power_dq(double q) const;

    static constexpr double kappa{60};

    bool is_gas_driven(double time) const;

  private:
    /** \brief is non-zero, if the connection is gas-controlled and zero, if
     * power-controlled.
     */
    Control<1> control;

    /** The set pressure for periods of gas-controlled operation. Values in
     * power-controlled periods are ignored.
     */
    Boundaryvalue<1> boundaryvalue;
    Power::Powernode *powerendnode{nullptr};

    double const gas2power_q_coefficient;
    double const power2gas_q_coefficient;
  };
} // namespace Model::Networkproblem::Gaspowerconnection
