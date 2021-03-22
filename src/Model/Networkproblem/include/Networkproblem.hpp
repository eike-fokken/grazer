#pragma once
#include <Eigen/Sparse>
#include <Subproblem.hpp>
#include <memory>
#include <vector>

namespace Network {
  class Net;
}

// This namespace holds all data relating to problems that construct the model
// equations from a network from the namespace Network.
namespace Model::Networkproblem {

  class Equationcomponent;

  // This class implements a Subproblem, that builds the model equations from a
  // network.
  class Networkproblem final : public Subproblem {

  public:

    std::string get_type() const override;

    ~Networkproblem() override; // override{};

    Networkproblem(std::unique_ptr<Network::Net> _network);
    
    void
    evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
             double new_time,
             Eigen::Ref<Eigen::VectorXd const> const &last_state,
             Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;
    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobian, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;

    void save_values(double time, Eigen::Ref<Eigen::VectorXd>new_state) final;

    void print_to_files(std::filesystem::path const &output_directory) final;


    virtual void set_initial_values(Eigen::Ref<Eigen::VectorXd>new_state,
                                    nlohmann::json initial_json) final;

  private:
    std::unique_ptr<Network::Net> network;
    std::vector<Equationcomponent *> equationcomponents;

    int reserve_indices(int const next_free_index) final;
  };

} // namespace Model::Networkproblem
