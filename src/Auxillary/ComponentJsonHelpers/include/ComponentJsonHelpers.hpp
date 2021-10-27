#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace Aux {

  std::vector<std::string>
  get_sorted_ids_of_json(nlohmann::json const &json, std::string key);
  /** \brief Sort all components in all their respective categories and
   * checks whether any id appears twice (in which case an exception is
   * thrown.)
   *
   * @param components The json to be sorted.
   * @param key The key inside the Networkproblem json, e.g. topology or
   * boundary.
   */
  void sort_json_vectors_by_id(nlohmann::json &components, std::string key);

  /** \brief Throws an exception if an id is used more than once
   * @param components The json to be checked.
   * @param key The key inside the Networkproblem json, e.g. topology or
   * boundary.
   */
  void check_for_duplicates(nlohmann::json &components, std::string key);
} // namespace Aux
