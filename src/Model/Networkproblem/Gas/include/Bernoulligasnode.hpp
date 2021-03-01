#pragma once
#include "Equationcomponent.hpp"
#include "Gasedge.hpp"
#include "Node.hpp"

namespace Model::Networkproblem::Gas {

  class Pipe;


  double bernoulli(Eigen::Vector2d p_qvol, Pipe const &pipe);

  Eigen::RowVector2d dbernoulli_dstate(Eigen::Vector2d const &p_qvol,
                                       Pipe const &pipe);

  class Bernoulligasnode : public Network::Node, public Equationcomponent {

  public:
    using Node::Node;

    /// Claims outer indices of attached edges.
    void setup() override;

    /// Returns zero, because the indices are owned by the attached edges, see
    /// setup().
    int get_number_of_states() const final;

    void print_to_files(std::filesystem::path const &) final{};

    void save_values(double, Eigen::Ref<Eigen::VectorXd const> const &) final{};

    void set_initial_values(Eigen::Ref<Eigen::VectorXd>,
                            nlohmann::ordered_json) final{};

  protected:
    void
    evaluate_flow_node_balance(Eigen::Ref<Eigen::VectorXd> rootvalues,
                               Eigen::Ref<Eigen::VectorXd const> const &state,
                               double prescribed_flow) const;

    void evaluate_flow_node_derivative(
        Aux::Matrixhandler *jacobianhandler,
        Eigen::Ref<Eigen::VectorXd const> const &state) const;

  private:
    std::vector<std::pair<int, Gasedge *>> directed_non_pipe_gas_edges;
    std::vector<std::pair<int, Pipe *>> directed_pipes;

    /// \brief number of state variables, this component needs.
    static constexpr int number_of_state_variables{0};
    };
  }
