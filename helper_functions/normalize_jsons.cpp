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

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::ordered_json;
namespace fs = std::filesystem;

int main() {
  std::vector<std::string> files
      = {"topology.json", "boundary.json", "initial.json", "control.json"};

  for (std::string const &file : files) {

    json input;
    {
      std::ifstream inputstream((fs::path(file)));
      inputstream >> input;
    }
    {
      std::ofstream outputstream((fs::path(file)));
      outputstream << input.dump(1, '\t');
    }
  }
}
