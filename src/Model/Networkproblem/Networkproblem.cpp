#include <Edge.hpp>
#include <Equationcomponent.hpp>
#include <Exception.hpp>
#include <Net.hpp>
#include <Networkproblem.hpp>
#include <Node.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <Eigen/Sparse>

namespace Model::Networkproblem {

  /// The constructor takes an instance of Net and finds out which Edges and
  /// Nodes actually hold equations to solve
  Networkproblem::Networkproblem(std::unique_ptr<Network::Net> _network)
      : network(std::move(_network)) {
    for (Network::Node *node : network->get_nodes()) {
      if (auto equationcomponent = dynamic_cast<Equationcomponent *>(node)) {
        equationcomponents.push_back(equationcomponent);
      }
    }

    for (Network::Edge *edge : network->get_edges()) {
      if (auto equationcomponent = dynamic_cast<Equationcomponent *>(edge)) {
        equationcomponents.push_back(equationcomponent);
      }
    }
  }

  void Networkproblem::evaluate(Eigen::VectorXd &rootfunction, double last_time,
                                double new_time,
                                Eigen::VectorXd const &last_state,
                                Eigen::VectorXd const &new_state) {
    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->evaluate(rootfunction, last_time, new_time, last_state,
                            new_state);
    }
  }

  void Networkproblem::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
      Eigen::VectorXd const &last_state, Eigen::VectorXd const &new_state) {
    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->evaluate_state_derivative(jacobianhandler, last_time,
                                             new_time, last_state, new_state);
    }
  }

  void Networkproblem::save_values(double time, Eigen::VectorXd &new_state) {
    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->save_values(time, new_state);
    }
  }

  void Networkproblem::print_to_files(std::filesystem::path &output_directory) {
    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->print_to_files(output_directory);
    }
  }

  void Networkproblem::display() const { network->display(); }

  void Networkproblem::set_initial_values(Eigen::VectorXd &new_state,
                                          nlohmann::ordered_json initial_json) {
    for (Equationcomponent *eqcomponent : equationcomponents) {
      auto idcomponent = dynamic_cast<Network::Idobject *>(eqcomponent);
      if (idcomponent == nullptr) {
        gthrow({"An equation component is not of type Idobject, which should "
                "never happen."});
      }
      auto component_id = idcomponent->get_id();
      auto finder = [component_id](nlohmann::ordered_json &x) {
        auto it = x.find("id");
        return it != x.end() and it.value() == component_id;
      };
      auto initjson =
          std::find_if(initial_json.begin(), initial_json.end(), finder);
      eqcomponent->set_initial_values(new_state, *initjson);
    }
  }

  int Networkproblem::reserve_indices(int const next_free_index) {
    int free_index = next_free_index;
    for (Equationcomponent *eqcomponent : equationcomponents) {
      free_index = eqcomponent->set_indices(free_index);
    }
    return free_index;
  }

} // namespace Model::Networkproblem
