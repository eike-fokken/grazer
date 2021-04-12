#include "Aux_json.hpp"
#include "Input_output.hpp"
#include "Problem.hpp"
#include "Timeevolver.hpp"
#include <Eigen/Sparse>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

int main(int argc, char **argv) {
  try {
    std::vector<std::string> cmd_arguments
        = Aux_executable::make_cmd_argument_vector(argc, argv);

    std::filesystem::path problem_data_file
        = Aux_executable::extract_input_data(cmd_arguments);

    std::filesystem::path output_dir
        = Aux_executable::prepare_output_dir("output");

    // This must be wrapped in exception handling code!

    auto all_json = aux_json::get_json_from_file_path(problem_data_file);

    auto time_evolution_json = all_json["time_evolution_data"];

    auto problem_json = all_json["problem_data"];

    // give the path information of the file:
    auto directory_path
        = std::filesystem::absolute(problem_data_file.parent_path());
    problem_json["GRAZER_file_directory"] = directory_path.string();

    auto initial_value_json = all_json["initial_values"];
    initial_value_json["GRAZER_file_directory"] = directory_path.string();
    Model::Timedata timedata(time_evolution_json);

    double tolerance = 1e-8;
    int maximal_number_of_newton_iterations = 50;
    Model::Timeevolver timeevolver(
        tolerance, maximal_number_of_newton_iterations);

    // This try block makes sure, the destructor of problem is called in order
    // to print out all data, we have already.
    Model::Problem problem(problem_json, output_dir, timeevolver.get_output());
    int number_of_states = problem.set_indices();
    std::cout << "data read" << std::endl;

    timeevolver.simulate(
        timedata, problem, number_of_states, initial_value_json);
  } catch (std::exception const &ex) {
    std::cout << "An exception was thrown!\n"
              << "All available data has been printed to output files\n"
              << "(unless printing failed, if so this is indicated above this "
                 "message.)\n"
              << "\nUse with caution!\n"
              << std::endl;
    std::cout << "The error message was: \n\n"
              << "###############################################\n"
              << ex.what()
              << "\n###############################################\n\n"
              << std::endl;
    return 1;
  } catch (...) {
    std::cout << "An unknown type of exception was thrown.\n\n"
                 "This is a bug and must be fixed!\n\n"
                 "Please contact the maintainer of Grazer!"
              << std::endl;
    return 1;
  }
  return 0;
}
