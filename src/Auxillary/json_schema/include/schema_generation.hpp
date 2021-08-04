#pragma once
#include <Componentfactory.hpp>
#include <Full_factory.hpp>
#include <any>
#include <deque>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

namespace Aux::schema {
  using Model::Componentfactory::Componentfactory;

  int make_full_factory_schemas(std::filesystem::path grazer_dir);

  void make_schemas(
      Componentfactory const &factory, std::filesystem::path schema_dir);
} // namespace Aux::schema
