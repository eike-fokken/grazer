#pragma once
#include "Equationcomponent.hpp"
#include "Gasedge.hpp"
#include "Node.hpp"
#include "Statecomponent.hpp"

namespace Model::Networkproblem::Gas {

  class Gasnode : public Statecomponent, public Network::Node {

  public:
    using Node::Node;

    /// Claims outer indices of attached edges.
    void setup() override;

    /// Returns zero, because the indices are owned by the attached edges, see
    /// setup().
    int get_number_of_states() const final;

    void print_to_files(std::filesystem::path const &) final{};

    void save_values(double, Eigen::Ref<Eigen::VectorXd const>) final{};

    void json_save(double, Eigen::Ref<Eigen::VectorXd const>) override{};

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd>, nlohmann::json const &) final{};

    /// \brief overloads the Equationcomponent static method. Returns false for
    /// gas nodes.
    ///
    /// returns false
    static bool needs_output_file();

  protected:
    void evaluate_flow_node_balance(
        Eigen::Ref<Eigen::VectorXd> rootvalues,
        Eigen::Ref<Eigen::VectorXd const> state, double prescribed_flow) const;

    void evaluate_flow_node_derivative(
        Aux::Matrixhandler *jacobianhandler,
        Eigen::Ref<Eigen::VectorXd const> state) const;

    std::vector<std::pair<int, Gasedge *>> directed_attached_gas_edges;

  private:
    /// \brief number of state variables, this component needs.
    static constexpr int number_of_state_variables{0};
  };
} // namespace Model::Networkproblem::Gas
