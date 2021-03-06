#pragma once
#include "Edge.hpp"
#include "Gasedge.hpp"

namespace Model::Networkproblem::Gas {

  class Shortcomponent : public Network::Edge, public Gasedge {

  public:
    using Edge::Edge;

    int get_number_of_states() const final;

    void setup() override;

    void new_print_helper(
        nlohmann::json &new_output, std::string const &component_type,
        std::string const &type);

    void json_save(double, Eigen::Ref<Eigen::VectorXd const>) override;

    void initial_values_helper(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json);

    /// Shortcomponents just use pressure and volumetric flow as their
    /// variables, so that this function simply returns the corresponding
    /// boundary state.
    Eigen::Vector2d get_boundary_p_qvol_bar(
        int direction, Eigen::Ref<Eigen::VectorXd const> state) const final;

    /// Because Shortcomponents use pressure and volumetric flow as their state
    /// variables, this function just hands `function_derivative` to
    /// jacobianhandler.
    void dboundary_p_qvol_dstate(
        int direction, Aux::Matrixhandler *jacobianhandler,
        Eigen::RowVector2d function_derivative, int rootvalues_index,
        Eigen::Ref<Eigen::VectorXd const> state) const final;

  private:
    /// \brief number of state variables, this component needs.
    static constexpr int number_of_state_variables{4};
  };

} // namespace Model::Networkproblem::Gas
