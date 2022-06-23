/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
#pragma once
#include <Componentfactory.hpp>
#include <Full_factory.hpp>
#include <any>
#include <deque>
#include <filesystem>
#include <string>

namespace Aux::schema {
  using Model::Componentfactory::Componentfactory;

  int make_full_factory_schemas(std::filesystem::path grazer_dir);

  void make_schemas(
      Componentfactory const &factory, std::filesystem::path schema_dir);
} // namespace Aux::schema
