#pragma once
#include <Componentfactory.hpp>
#include <Full_factory.hpp>
#include <deque>
#include <any>
#include <string>
#include <filesystem>

namespace Aux::schema {
  using Model::Componentfactory::Componentfactory;

  int make_full_factory_schemas(std::deque<std::string> args, std::map<std::string, std::any> /*kwargs*/);

  void make_schemas(
      Componentfactory const &factory, std::filesystem::path schema_dir);
} // namespace Aux::schema
