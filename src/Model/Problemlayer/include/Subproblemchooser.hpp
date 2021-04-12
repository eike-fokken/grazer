#pragma once
#include "Subproblem.hpp"
#include <memory>
#include <nlohmann/json.hpp>

namespace Model {

  std::unique_ptr<Subproblem> build_subproblem(
      std::string subproblem_type, nlohmann::json &subproblem_json,
      std::filesystem::path const &output_dir, nlohmann::json &output_json);

  std::unique_ptr<Subproblem> build_networkproblem(
      nlohmann::json &subproblem_json, std::filesystem::path const &output_dir,
      nlohmann::json &output_json);
} // namespace Model
