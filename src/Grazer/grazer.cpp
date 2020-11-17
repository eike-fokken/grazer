#include <Exception.hpp>
#include <Jsonreader.hpp>
#include <Problem.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char **argv) {

  if (argc != 4) {
    gthrow({" Wrong number of arguments."})
  }

  if (!std::filesystem::is_regular_file(argv[1]) or
      !std::filesystem::is_regular_file(argv[2]) or
      !std::filesystem::is_regular_file(argv[3])) {
    std::cout
        << "One of the given files is not a regular file, I will abort now."
        << std::endl;
    return 1;
  }

  std::filesystem::path output_dir("output");

  if (std::filesystem::exists(output_dir)) {
    if (!std::filesystem::is_directory(output_dir)) {
      std::cout << "The output directory \"output\"\n"
                   "is present, but not a directory, I will abort now."
                << std::endl;
      return 1;
    }
    auto ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    std::string milli = std::to_string(ms_since_epoch.count());
    std::string moved_output_dir;
    moved_output_dir = output_dir;
    moved_output_dir.append("_");
    moved_output_dir.append(milli);
    std::filesystem::rename(output_dir, moved_output_dir);
    std::cout << "moved old directory " << output_dir << " to "
              << moved_output_dir << std::endl;
    std::cout << "result files will be saved to " << output_dir << std::endl;
  }
  std::filesystem::create_directory(output_dir);
  std::cout << output_dir << std::endl;

  auto p = Jsonreader::setup_problem(argv[1], argv[3]);

  Eigen::VectorXd initial_state;
  Jsonreader::set_initial_values(initial_state, argv[2], *p);

  int number = p->set_indices();
  std::cout << number;
  // p->display();
}
