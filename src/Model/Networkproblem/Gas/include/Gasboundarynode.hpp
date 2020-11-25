#include <Equationcomponent.hpp>
#include <Node.hpp>
#include <nlohmann/json.hpp>
#include <Boundaryvalue.hpp>
#include <string>

namespace Model::Networkproblem::Gas {

  /// This enum type distinguishes pressure boundaries and flow boundaries.
  enum Boundarytype {pressure, flow, negflow};


class Gasboundarynode final: public Network::Node,
               public Model::Networkproblem::Equationcomponent {

public:
  Gasboundarynode(std::string _id, nlohmann::ordered_json boundary_json,
         nlohmann::ordered_json topology_json);

  virtual ~Gasboundarynode() {};

  void set_boundary_condition(nlohmann::ordered_json boundary_json);

    void evaluate(Eigen::VectorXd & rootfunction, double last_time,
                  double new_time, Eigen::VectorXd const &last_state,
                  Eigen::VectorXd const &new_state) override;
    void evaluate_state_derivative(
        Aux::Matrixhandler * jacobianhandler, double last_time, double new_time,
        Eigen::VectorXd const &, Eigen::VectorXd const &new_state) override;

    void display() const override;

    int get_number_of_states() const override;

    void print_to_files(std::filesystem::path const &) override{};

    void save_values(double, Eigen::VectorXd const &) override{};

    void set_initial_values(Eigen::VectorXd &, nlohmann::ordered_json)
        override{};


private:
  
  Boundaryvalue<Gasboundarynode, 1> boundaryvalue;
  Boundarytype btype;
  };

} // namespace Model::Networkproblem::Gas
