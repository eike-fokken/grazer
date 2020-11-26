#include <Gasedge.hpp>

namespace Model::Networkproblem::Gas {

  class Shortpipe final: public Gasedge<2> {

  public:
    using Gasedge<2>::Gasedge;


    void evaluate(Eigen::VectorXd &rootfunction, double last_time,
                          double new_time, Eigen::VectorXd const &last_state,
                          Eigen::VectorXd const &new_state) const override;
    void evaluate_state_derivative( Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
                                           Eigen::VectorXd const &, Eigen::VectorXd const &new_state) const override;

    //maybe we should override display?
    //void display() const override;

    int get_number_of_states() const override;

    
    void print_to_files(std::filesystem::path const &output_directory) override;

    void save_values(double time, Eigen::VectorXd const &state) override;

    void set_initial_values(Eigen::VectorXd &new_state,
                            nlohmann::ordered_json initial_json) override;


  };

}


