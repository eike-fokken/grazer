#pragma once
#include <Componentfactory.hpp>
#include <filesystem>

namespace Aux::schema {
  using Model::Componentfactory::Componentfactory;
  void make_schemas(
      Componentfactory const &factory, std::filesystem::path schema_dir);
} // namespace Aux::schema
