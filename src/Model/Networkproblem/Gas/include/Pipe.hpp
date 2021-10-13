#pragma once
#include "Edge.hpp"
#include "Gasedge.hpp"

namespace Model::Balancelaw {
  class Pipe_Balancelaw;
}

namespace Model::Scheme {
  class Threepointscheme;
}

namespace Model::Gas {

  class Pipe final : public Gasedge, public Network::Edge {
  public:
    static std::string get_type();
    std::string get_gas_type() const override;
    static int init_vals_per_interpol_point();
    static nlohmann::json get_schema();
    static nlohmann::json get_initial_schema();

    Pipe(
        nlohmann::json const &topology,
        std::vector<std::unique_ptr<Network::Node>> &nodes);

    ~Pipe() override;

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;
    void d_evalutate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;
    void d_evalutate_d_last_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;
    void setup() override;

    int needed_number_of_states() const override;

    void add_results_to_json(nlohmann::json &new_output) override;

    void json_save(
        double time, Eigen::Ref<Eigen::VectorXd const> const &state) override;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json) override;

    Eigen::Vector2d get_boundary_p_qvol_bar(
        int direction,
        Eigen::Ref<Eigen::VectorXd const> const &state) const override;

    void dboundary_p_qvol_dstate(
        int direction, Aux::Matrixhandler &jacobianhandler,
        Eigen::RowVector2d function_derivative, int rootvalues_index,
        Eigen::Ref<Eigen::VectorXd const> const &state) const override;

  private:
    double get_length();
    int const number_of_points;
    double const Delta_x;

    std::unique_ptr<Balancelaw::Pipe_Balancelaw const> bl;
    std::unique_ptr<Scheme::Threepointscheme const> scheme;
  };

} // namespace Model::Gas
