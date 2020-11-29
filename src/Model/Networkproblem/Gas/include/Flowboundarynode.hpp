#include <Boundaryvalue.hpp>
#include <Equationcomponent.hpp>
#include <Gasnode.hpp>
#include <Node.hpp>
#include <nlohmann/json.hpp>
#include <string>

namespace Model::Networkproblem::Gas {

class Flowboundarynode final: public Gasnode {

public:
  Flowboundarynode(std::string _id, nlohmann::ordered_json boundary_json,
         nlohmann::ordered_json topology_json);

  virtual ~Flowboundarynode() {};

  
    void evaluate(Eigen::VectorXd & rootfunction, double last_time,
                  double new_time, Eigen::VectorXd const &last_state,
                  Eigen::VectorXd const &new_state) const override;
    void evaluate_state_derivative(
        Aux::Matrixhandler * jacobianhandler, double last_time, double new_time,
        Eigen::VectorXd const &, Eigen::VectorXd const &new_state) const override;

    void display() const override;


private:
  
  Boundaryvalue<Flowboundarynode, 1> boundaryvalue;
  };

} // namespace Model::Networkproblem::Gas
