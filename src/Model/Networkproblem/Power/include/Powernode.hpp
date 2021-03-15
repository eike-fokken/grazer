#pragma once
#include <Boundaryvalue.hpp>
#include <Eigen/Sparse>
#include <Equationcomponent.hpp>
#include <Node.hpp>
#include <nlohmann/json.hpp>

namespace Model::Networkproblem::Power {

  class Powernode : public Equationcomponent, public Network::Node {

  public:
    Powernode(nlohmann::json const & topology);

    virtual ~Powernode(){};

    void setup() override;

    int get_number_of_states() const final;

    double get_G() const;
    double get_B() const;

    void print_to_files(std::filesystem::path const &output_directory) final;

    void set_initial_values(Eigen::Ref<Eigen::VectorXd>new_state,
                            nlohmann::ordered_json initial_json) final;

    double P(Eigen::Ref<Eigen::VectorXd const> const &new_state) const;
    double Q(Eigen::Ref<Eigen::VectorXd const> const &new_state) const;
    void evaluate_P_derivative(int equationindex, Aux::Matrixhandler *jacobianhandler,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const;
    void evaluate_Q_derivative(int equationindex, Aux::Matrixhandler *jacobianhandler,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const;

  protected:

    /// \brief saves the values of a power node for later printout into files.
    ///
    /// As the method for obtaining P_val and Q_val depend on the actual type, this is just a helper function
    /// that is called from the respective Powernode.
    void save_power_values(double time, Eigen::Ref<Eigen::VectorXd const> const &state, double P_val, double Q_val);

    

    Boundaryvalue<2> const boundaryvalue;
    /// Real part of the admittance of this node
    double G;
    /// Imaginary part of the admittance of this node
    double B;

  private:
    /// \brief number of state variables, this component needs.
    static constexpr int number_of_state_variables{2};

    std::vector<std::tuple<double,double,Powernode*>> attached_component_data;

  };

} // namespace Model::Networkproblem::Power
