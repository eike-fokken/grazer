#include "Subproblemchooser.hpp"
#include "Exception.hpp"
#include "Full_factory.hpp"
#include "Netfactory.hpp"
#include "Networkproblem.hpp"

namespace Model {

  std::unique_ptr<Subproblem> build_subproblem(
      std::string subproblem_type, nlohmann::json &subproblem_json,
      std::filesystem::path const &output_dir) {
    if (subproblem_type == "Network_problem") {
      return build_networkproblem(subproblem_json, output_dir);
    } else {
      gthrow(
          {"Encountered unknown subproblem type ", subproblem_type,
           ". Aborting now."});
    }
  }

  std::unique_ptr<Subproblem> build_networkproblem(
      nlohmann::json &networkproblem_json,
      std::filesystem::path const &output_dir) {
    Componentfactory::Full_factory componentfactory;

    auto net_ptr = Networkproblem::build_net(
        networkproblem_json, componentfactory, output_dir);
    return std::make_unique<Networkproblem::Networkproblem>(std::move(net_ptr));
  }
} // namespace Model
