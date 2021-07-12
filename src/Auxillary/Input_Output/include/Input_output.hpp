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

#pragma once
#include <filesystem>
#include <string>
#include <tuple>
#include <vector>

namespace io {
  /// Checks whether the given path filepath is inside of problem_root_path.
  ///
  /// If yes, returns a normalized path.
  /// If no, throws.
  bool absolute_file_path_in_root(
      const std::filesystem::path &problem_root_path,
      const std::filesystem::path &filepath);

  /// Checks, whether the given output directory path is present.
  ///
  /// If not,
  /// returns the path. If yes, checks whether it is a directory. If yes, moves
  /// this directory and appends a unique string to it. If it is present, but
  /// not a directory, it throws an exception.
  std::filesystem::path prepare_output_dir(std::filesystem::path output_dir);

} // namespace io
