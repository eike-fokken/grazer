#pragma once
#include "Edge.hpp"
#include <Gasedge.hpp>


namespace Model::Networkproblem::Power {
  class Powernode;  
} // namespace Model::Networkproblem::Power

namespace Model::Networkproblem::Gas {

    class Gaspowerconnection final : public Network::Edge, public Gasedge {
    public:
      static std::string get_type();
      static nlohmann::json get_schema();

      Gaspowerconnection(nlohmann::json const &topology,
           std::vector<std::unique_ptr<Network::Node>> &nodes);

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

      Eigen::Vector2d get_boundary_p_qvol_bar(
          int direction,
          Eigen::Ref<Eigen::VectorXd const> const &state) const override;

      void dboundary_p_qvol_dstate(
          int direction, Aux::Matrixhandler *jacobianhandler,
          Eigen::RowVector2d function_derivative, int rootvalues_index,
          Eigen::Ref<Eigen::VectorXd const> const &state) const override;

      double smoothing_polynomial(double q) const;
      double dsmoothing_polynomial_dq(double q) const;

      double generated_power(double q) const;
      double dgenerated_power_dq(double q) const;

      static constexpr double kappa{60};
    private:

      Model::Networkproblem::Power::Powernode * powerendnode{nullptr};

      double const gas2power_q_coefficient;
      double const power2gas_q_coefficient;


      };
  }
