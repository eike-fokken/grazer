/*
 * Grazer - network simulation tool
 *
 * Copyright 2020-2021 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	AGPL-3.0-or-later
 *
 * Licensed under the GNU Affero General Public License v3.0, found in
 * LICENSE.txt and at https://www.gnu.org/licenses/agpl-3.0.html
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
