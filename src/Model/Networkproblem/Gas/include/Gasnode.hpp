#pragma once
#include "Equationcomponent.hpp"
#include "Gasedge.hpp"
#include "Node.hpp"

namespace Model::Gas {

  class Gasnode : public Equationcomponent, public Network::Node {

  public:
    using Node::Node;

    /**
     * @brief Claims outer indices of attached edges.
     *
     */
    void setup() final;

  protected:
    void evaluate_flow_node_balance(
        Eigen::Ref<Eigen::VectorXd> rootvalues,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        double prescribed_flow) const;

    void evaluate_flow_node_derivative(
        Aux::Matrixhandler &jacobianhandler,
        Eigen::Ref<Eigen::VectorXd const> const &state) const;

    std::vector<std::pair<int, Gasedge *>> directed_attached_gas_edges;

  private:
    static constexpr int number_of_state_variables{0}; /**< number of state variables, this component needs. */
  };
} // namespace Model::Gas
