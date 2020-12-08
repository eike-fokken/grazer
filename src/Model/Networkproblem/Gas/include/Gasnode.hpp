#pragma once
#include "Gasedge.hpp"
#include <Node.hpp>
#include <Equationcomponent.hpp>



namespace Model::Networkproblem::Gas {

  class Gasnode: public Network::Node, public Equationcomponent {

  public:

    using Node::Node;

    /// Claims outer indices of attached edges.
    void setup() override;

    /// Returns zero, because the indices are owned by the attached edges, see
    /// setup().
    int get_number_of_states() const final;

    void print_to_files(std::filesystem::path const &) final{};

    void save_values(double, Eigen::Ref<Eigen::VectorXd const> const &) final{};

    void set_initial_values(Eigen::Ref<Eigen::VectorXd>, nlohmann::ordered_json)
      final{};

  protected:
    void evaluate_flow_node_balance(Eigen::Ref<Eigen::VectorXd> rootvalues,
                                    Eigen::Ref<Eigen::VectorXd const> const &state,
                                    double prescribed_flow) const;
    void evaluate_pressure_node_balance(Eigen::Ref<Eigen::VectorXd> rootvalues,
                                        Eigen::Ref<Eigen::VectorXd const> const &state,
                                        double prescribed_pressure) const;

    void evaluate_flow_node_derivative(Aux::Matrixhandler *jacobianhandler,
                                       Eigen::Ref<Eigen::VectorXd const> const &state) const;
    void evaluate_pressure_node_derivative(Aux::Matrixhandler *jacobianhandler,
                                           Eigen::Ref<Eigen::VectorXd const> const &) const;

    std::vector<std::pair<int, Gasedge *>> directed_attached_gas_edges;
  };
}
