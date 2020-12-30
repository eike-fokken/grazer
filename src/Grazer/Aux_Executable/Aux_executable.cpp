#include <Aux_executable.hpp>
#include <Exception.hpp>
#include <iostream>
#include <tuple>

namespace Aux_executable {

  std::filesystem::path prepare_output_dir(std::string output_dir_string){

    std::filesystem::path output_dir(output_dir_string);

    if (std::filesystem::exists(output_dir)) {
      if (!std::filesystem::is_directory(output_dir)) {
        gthrow({"The output directory, \"", output_dir_string ,"\" is present, but not a directory, I will abort now."});
      }
      auto ms_since_epoch =
          std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::system_clock::now().time_since_epoch());
      std::string milli = std::to_string(ms_since_epoch.count());
      std::string moved_output_dir;
      moved_output_dir = output_dir.string();
      moved_output_dir.append("_");
      moved_output_dir.append(milli);
      std::filesystem::rename(output_dir.string(), moved_output_dir);
      std::cout << "moved old directory " << output_dir << " to "
                << moved_output_dir << std::endl;
      std::cout << "result files will be saved to " << output_dir << std::endl;
    }
    std::filesystem::create_directory(output_dir);
    return output_dir;
  }

  std::tuple<std::filesystem::path, std::filesystem::path,
             std::filesystem::path, double, double,
             double>
  extract_input_data(std::vector<std::string> cmd_arguments){
asdf
    std::filesystem::path topology("");
    std::filesystem::path initial("");
    std::filesystem::path boundary("");

    double delta_t = 0.;
    double delta_x = 0.;
    double T = 0.;

    return std::make_tuple(topology,initial,boundary,delta_t,delta_x,T);
  }

  std::tuple<std::filesystem::path, std::filesystem::path,
             std::filesystem::path>
  prepare_inputfiles(std::vector<std::string> input_filenames)
  {    if (input_filenames.size()!=3) {
      gthrow({"Function ",__FUNCTION__, " can only be called with exactly three input filenames."});
        }

        std::filesystem::path topology(input_filenames[0]);
        std::filesystem::path initial(input_filenames[1]);
        std::filesystem::path boundary(input_filenames[2]);

        if (!std::filesystem::is_regular_file(topology) or
            !std::filesystem::is_regular_file(initial) or
            !std::filesystem::is_regular_file(boundary)) {
          gthrow({"One of the given files is not a regular file, I will abort "
                  "now."});
      }
    } else if (argc == 1) {
      topology = "topology_pretty.json";
      initial = "initial_pretty.json";
      boundary = "boundary_pretty.json";
    } else {
      gthrow(
          {"Wrong number of arguments, I will abort now.\n Correct number of "
           "arguments is 0, 3 or 6."});
    }

    double Delta_t = 1800;
    double Delta_x = 10000;
    double T = 86400;
    if (argc == 7) {
      Delta_t = std::stod(argv[4]);
      Delta_x = std::stod(argv[5]);
      T = std::stod(argv[6]);
    }
  }
}
