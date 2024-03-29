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
#include <filesystem>
#include <nlohmann/json.hpp>

namespace aux_json {
  /// \brief replaces a filename string with the json from the file
  ///
  /// Checks whether super_json["key"] holds a string. If so, checks that the
  /// string is a valid file name, reads the contents of the file of that name
  /// into a json object and replaces super_json["key"] with this json object.
  /// if super_json["key"] is a json object, does nothing.
  void replace_entry_with_json_from_file(
      nlohmann::json &super_json, std::string const &key);

  /// \brief Constructs a json object from a file.
  ///
  /// @throws std::runtime_error if the file is not present or if is not valid
  /// json.
  nlohmann::json
  get_json_from_file_path(std::filesystem::path const &file_path);

  /// \brief Constructs an ordered json object from a file.
  ///
  /// @throws std::runtime_error if the file is not present or if is not valid
  /// json.
  nlohmann::ordered_json
  get_ordered_json_from_file_path(std::filesystem::path const &file_path);

  void overwrite_json(
      std::filesystem::path const &file_path,
      nlohmann::ordered_json const &new_json);

} // namespace aux_json
