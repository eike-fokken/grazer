#pragma once
#include <filesystem>
#include <functional>
#include <string>
#include <tuple>
#include <vector>

/** \brief Namespace concerned with preparing files for writing to.
 */
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
   */
  std::filesystem::path
  attach_epoch_count(std::filesystem::path const &filename);

  /** Function pointer type for a function like attach_epoch_count,
   * returns possible filenames, usually differing ones.
   */
  using filename_generator = std::filesystem::path (*)(
      std::filesystem::path const &original_filename);

  /**  Creates a unique filepath, by repeatedly calling generator and testing
   * whether the given filename doesn't exist yet.
   *
   * @param filename filename to modify to get a unique filename.
   * @param generator A function of type \ref io::filename_generator
   */
  std::filesystem::path create_new_output_file(
      std::filesystem::path const &filename, filename_generator generator);

  /** Creates a unique json file in directory/output/ for program output
   *
   *  A unique name is found by attaching the number of milliseconds since the
   *  unix epoch to the filename "output". If this name is already taken, it
   *  sleeps for at least one millisecond and tries again. Throws if 100 tries
   *  fail.
   *  @throws if directory/output/ is present but not a directory.
   *  @throws if 100 tries of finding a unique name fail.
   *  @returns a path to the unique json file.
   */

  std::filesystem::path prepare_output_file(std::filesystem::path directory);

  /** \brief Returns a string encoding the current time up to millisecond
   * precision.
   *
   * Format is: YYYY.MM.DD_hh:mm:ss.mmm
   * The last three m are milliseconds.
   */
  std::string millisecond_datetime_timestamp();

  /** Function pointer type for a function millisecond_datetime_timestamp,
   * returns possible directory names, usually differing ones.
   */
  using dirname_generator = std::string (*)();

  /** \brief Creates a unique output directory named by dirname_generator.
   *
   * If the directory is already present, waits one millisecond and tries again.
   * Aborts after 20 trials.
   * This function is useful, if many instances of grazer are called in
   * parallel, such as when many samples are used in a Monte-Carlo simulation.
   * It makes sure that each instance writes into its own output directory.
   *
   * @param outer_output_directory The directory that should be the direct
   * parent of the newly created directory
   * param dirname_generator function to produce a directory name
   * @returns A path to the newly created directory
   */

  std::filesystem::path unique_output_directory(
      std::filesystem::path const &outer_output_directory,
      dirname_generator dirname_generator);

  /** \brief Fills the output directory with the input files and opens empty
   * output files inside.
   * @param output_directory The directory to fill
   * @param problem_data_directory The directory to copy here
   */
  void prepare_output_directory(
      std::filesystem::path const &output_directory,
      std::filesystem::path const &problem_data_directory,
      std::vector<std::string> filenames_to_create);
} // namespace io
