#pragma once
#include <filesystem>
#include <functional>
#include <string>
#include <tuple>
#include <vector>

namespace io {

  /** Checks whether the given path filepath is inside of problem_root_path.
   *
   * If yes, returns a normalized path.
   * If no, throws.
   */
  bool absolute_file_path_in_root(
      const std::filesystem::path &problem_root_path,
      const std::filesystem::path &filepath);

  /** attaches the number of milliseconds since 1970 as a string to its argument
   * filename.
   *
   */
  std::filesystem::path
  attach_epoch_count(std::filesystem::path const &filename);

  /** Function pointer type for a function like attach_epoch_count.
   */
  using filename_generator = std::filesystem::path (*)(
      std::filesystem::path const &original_filename);

  /**  Creates a unique filepath, by repeatedly calling generator and testing
   * whether the given filename doesn't exist yet.
   *
   * @param filename filename to modify to get a unique filename.
   * @param generator
   */
  std::filesystem::path create_new_output_file(
      std::filesystem::path const &filename, filename_generator generator);

  /** Creates a unique json file in directory/output/ for program output
   *
   *  A unique name is found by attaching the number of milliseconds since the
   *  unique epoch to the filename "output". If this name is already taken, it
   *  sleeps for at least one millisecond and tries again. Throws if 100 tries
   *  fail.
   *  @throws if directory/output/ is present but not a directory.
   *  @throws if 100 tries of finding a unique name fail.
   *  @returns a path to the unique json file.
   */

  std::filesystem::path prepare_output_file(std::filesystem::path directory);

} // namespace io
