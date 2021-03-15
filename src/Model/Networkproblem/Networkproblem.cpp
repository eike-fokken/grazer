#include "Networkproblem.hpp"
#include "Edge.hpp"
#include "Equationcomponent.hpp"
#include "Exception.hpp"
#include "Net.hpp"
#include "Node.hpp"
#include <Eigen/Sparse>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>
// #include <execution>
namespace Model::Networkproblem {

  std::string Networkproblem::get_type() const {return "Network_problem";}

  Networkproblem::~Networkproblem() {}

  /// The constructor takes an instance of Net and finds out which Edges and
  /// Nodes actually hold equations to solve
  Networkproblem::Networkproblem(std::unique_ptr<Network::Net> _network): network(std::move(_network)) {
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



      void Networkproblem::evaluate(
          Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
          double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
          Eigen::Ref<Eigen::VectorXd const> const &new_state) const {

    // // This should evaluate in parallel, test on bigger problems later on:
    // std::for_each(std::execution::par_unseq, equationcomponents.begin(),
    //               equationcomponents.end(),[&](auto&& eqcomponent)
    // {
    // eqcomponent->evaluate(rootvalues, last_time, new_time, last_state,
    //                       new_state);
    // } 
    // );
    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->evaluate(rootvalues, last_time, new_time, last_state,
                            new_state);
    }
  }

  void Networkproblem::evaluate_state_derivative(
      ::Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state, Eigen::Ref<Eigen::VectorXd const> const &new_state) const {

    // // This should evaluate in parallel, test on bigger problems later on:
    // std::for_each(std::execution::par_unseq, equationcomponents.begin(),
    //               equationcomponents.end(), [&](auto &&eqcomponent) {
    //                 eqcomponent->evaluate_state_derivative(
    //                     jacobianhandler, last_time, new_time, last_state,
    //                     new_state);
    //               });

    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->evaluate_state_derivative(jacobianhandler, last_time,
                                             new_time, last_state, new_state);
    }
  }

  void Networkproblem::save_values(double time, Eigen::Ref<Eigen::VectorXd>new_state) {
    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->save_values(time, new_state);
    }
  }

  void Networkproblem::print_to_files(
      std::filesystem::path const &output_directory) {
    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->print_to_files(output_directory);
    }
  }

  void Networkproblem::set_initial_values(Eigen::Ref<Eigen::VectorXd>new_state,
                                          nlohmann::json initial_json) {

    std::vector<Network::Idobject* > idcomponents;
    idcomponents.reserve(equationcomponents.size());

    for (Equationcomponent *eqcomponent : equationcomponents) {
      auto idcomponent = dynamic_cast<Network::Idobject *>(eqcomponent);
      if (idcomponent == nullptr) {
        gthrow({"An equation component is not of type Idobject, which should "
            "never happen."});
      }
      idcomponents.push_back(idcomponent);
    }

    for (auto const &component : {"nodes", "connections"}) {
      for (auto const & componenttype : initial_json[component]) {
        for (auto const & componentjson: componenttype){
          auto component_id = componentjson["id"];
          auto find_id = [component_id](Network::Idobject const * x) {
            return component_id == x->get_id();
          };
          auto iterator = std::find_if(idcomponents.begin(),idcomponents.end(),find_id);
          auto index = iterator - idcomponents.begin();
          equationcomponents[static_cast<unsigned long>(index)]->set_initial_values(new_state, componentjson);
        }
      }
    }
  }

  int Networkproblem::reserve_indices(int const next_free_index) {
    int free_index = next_free_index;
    for (Equationcomponent *eqcomponent : equationcomponents) {
      free_index = eqcomponent->set_indices(free_index);
    }
    for (Equationcomponent *eqcomponent : equationcomponents) {
      eqcomponent->setup();
    }

    return free_index;
  }

} // namespace Model::Networkproblem
