#pragma once
#include <Gasedge.hpp>

namespace Model::Networkproblem::Power {
  class Powernode;  
} // namespace Model::Networkproblem::Power

  namespace Model::Networkproblem::Gas {

    class Gaspowerconnection final : public Gasedge {
    public:
      Gaspowerconnection(std::string _id, Network::Node *start_node,
                         Network::Node *end_node,
                         nlohmann::ordered_json topology_json);

      void evaluate(
          Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
          double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
          Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;
      void evaluate_state_derivative(
          Aux::Matrixhandler *jacobianhandler, double last_time,
          double new_time, Eigen::Ref<Eigen::VectorXd const> const &,
          Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;

      void setup() override;

      int get_number_of_states() const override;

      void
      print_to_files(std::filesystem::path const &output_directory) override;

      void save_values(double time,
                       Eigen::Ref<Eigen::VectorXd const> const &state) override;

      void set_initial_values(Eigen::Ref<Eigen::VectorXd> new_state,
                              nlohmann::ordered_json initial_json) override;

      Eigen::Vector2d get_boundary_p_qvol(
          int direction,
          Eigen::Ref<Eigen::VectorXd const> const &state) const override;

      void dboundary_p_qvol_dstate(
          int direction, Aux::Matrixhandler *jacobianhandler,
          Eigen::RowVector2d function_derivative, int rootvalues_index,
          Eigen::Ref<Eigen::VectorXd const> const &state) const override;

    private:
      double smoothing_polynomial(double q) const;
      double dsmoothing_polynomial_dq(double q) const;

      double generated_power(double q) const;
      double dgenerated_power_dq(double q) const;

      Model::Networkproblem::Power::Powernode * powerendnode{nullptr};

      double const gas2power_q_coefficient;
      double const power2gas_q_coefficient;

        static constexpr double kappa{1e-4};
      };
  }
