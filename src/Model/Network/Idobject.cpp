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
#include "make_schema.hpp"
#include <Idobject.hpp>
#include <iostream>

namespace Network {

  nlohmann::json Idobject::get_schema() {
    nlohmann::json schema;
    schema["type"] = "object";
    Aux::schema::add_required(schema, "id", Aux::schema::type::string());
    return schema;
  }

  Idobject::Idobject(std::string const &_id) :
      idptr(std::make_unique<std::string const>(_id)) {}

  std::string const &Idobject::get_id() const { return (*idptr); }

  std::string Idobject::get_id_copy() const { return (*idptr); }

} // namespace Network
