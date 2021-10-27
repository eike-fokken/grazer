#include "ComponentJsonHelpers.hpp"
#include "Exception.hpp"
namespace Aux {

  std::vector<std::string>
  get_sorted_ids_of_json(nlohmann::json const &json, std::string key) {
    std::vector<std::string> sorted_component_id_vector;
    for (auto const &component_class : {"nodes", "connections"}) {
      if (not json.contains(component_class)) {
        continue;
      }
      for (auto &[component_type, sub_json] : json[component_class].items()) {
        for (auto &component : sub_json) {
          if (not component.contains("id")) {
            gthrow({
                "The json inside of the following object",
                " does not contain an id! ",
                component.dump(1, '\t'),
                "\nThe object was inside \n\n",
                key,
                "[",
                component_class,
                "][",
                component_type,
                "]\n\n",
                "Aborting...",
            });
          }
          sorted_component_id_vector.push_back(
              component["id"].get<std::string>());
        }
      }
    }
    std::sort(
        sorted_component_id_vector.begin(), sorted_component_id_vector.end());
    return sorted_component_id_vector;
  }

  void check_for_duplicates(nlohmann::json &components, std::string key) {
    auto component_vector = get_sorted_ids_of_json(components, key);

    auto first_eq_id
        = std::adjacent_find(component_vector.begin(), component_vector.end());
    if (first_eq_id != component_vector.end()) {
      gthrow({"The id ", (*first_eq_id), " appears twice in ", key, " ."});
    }
  }

  void sort_json_vectors_by_id(nlohmann::json &components, std::string key) {

    check_for_duplicates(components, key);

    for (auto const &component_class : {"nodes", "connections"}) {
      if (not components.contains(component_class)) {
        continue;
      }
      for (auto &[component_type, sub_json] :
           components[component_class].items()) {
        auto &second_json_vector_json
            = components[component_class][component_type];
        // Define < function
        auto id_compare_less
            = [](nlohmann::json const &a, nlohmann::json const &b) -> bool {
          return a["id"].get<std::string>() < b["id"].get<std::string>();
        };
        std::sort(
            second_json_vector_json.begin(), second_json_vector_json.end(),
            id_compare_less);
      }
    }
  }
} // namespace Aux
