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

#include <Exception.hpp>
#include <numeric>
#include <sstream>
#include <stdexcept>

void exception_builder(
    std::vector<std::string> stringvector, char const *file, int line) {
  std::string rawmsg = std::accumulate(
      stringvector.begin(), stringvector.end(), std::string(""));
  std::ostringstream o;
  o << file << ":" << line << ": " << rawmsg;
  std::string msg = o.str();
  throw std::runtime_error(msg);
}
