#include "Aux_json.hpp"
#include "Input_output.hpp"
#include "Problem.hpp"
#include "Timeevolver.hpp"
#include "schema_generation.hpp"
#include "schema_key_insertion.hpp"
#include <Eigen/Sparse>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <structopt/structopt.hpp>

static int grazer_run(std::filesystem::path directory_path) {
  auto problem_directory = directory_path / "problem";
  auto problem_data_file = problem_directory / "problem_data.json";
  auto output_dir = io::prepare_output_dir(directory_path / "output");

  // This must be wrapped in exception handling code!

  auto all_json = aux_json::get_json_from_file_path(problem_data_file);

  auto time_evolution_json = all_json["time_evolution_data"];
  auto problem_json = all_json["problem_data"];

  // give the path information of the file:
  problem_json["GRAZER_file_directory"] = problem_directory.string();

  auto initial_value_json = all_json["initial_values"];
  initial_value_json["GRAZER_file_directory"] = problem_directory.string();
  Model::Timedata timedata(time_evolution_json);

  double tolerance = 1e-8;
  int maximal_number_of_newton_iterations = 50;
  Model::Timeevolver timeevolver(
      tolerance, maximal_number_of_newton_iterations, output_dir);

  Model::Problem problem(problem_json, timeevolver.get_output_dir());
  int number_of_states = problem.set_indices();
  std::cout << "data read" << std::endl;

  timeevolver.simulate(timedata, problem, number_of_states, initial_value_json);
  return 0;
}

struct Grazer {
  struct Run : structopt::sub_command {
    std::filesystem::path grazer_directory;
  };
  struct Schema : structopt::sub_command {
    struct Make_Full_Factory : structopt::sub_command {
      std::filesystem::path grazer_directory;
    };

    struct Insert_Link : structopt::sub_command {
      std::filesystem::path grazer_directory;
    };

    Make_Full_Factory make_full_factory;
    Insert_Link insert_link;
  };

  Run run;
  Schema schema;
};
STRUCTOPT(Grazer::Run, grazer_directory);
STRUCTOPT(Grazer::Schema::Make_Full_Factory, grazer_directory);
STRUCTOPT(Grazer::Schema::Insert_Link, grazer_directory);
STRUCTOPT(Grazer::Schema, make_full_factory, insert_link);
STRUCTOPT(Grazer, run, schema);

int main(int argc, char **argv) {
  auto app = structopt::app("grazer");
  try {
    auto args = app.parse<Grazer>(argc, argv);

    // run?
    if (args.run.has_value()) {
      return grazer_run(args.run.grazer_directory);
    } else if (args.schema.has_value()) {
      if (args.schema.make_full_factory.has_value()) {
        return Aux::schema::make_full_factory_schemas(
            args.schema.make_full_factory.grazer_directory);
      } else if (args.schema.insert_link.has_value()) {
        return Aux::schema::insert_schema_key_into_data_jsons(
            args.schema.insert_link.grazer_directory);
      }
    }
  } catch (std::exception const &ex) {
    std::cout << "[Error]: " << ex.what() << std::endl;
  } catch (...) {
    std::cout << "An unkown type of exception was thrown.\n\n"
                 "This is a bug and must be fixed!\n\n"
              << std::endl;
  }
  std::cout << app.help() << std::endl;
  return 1;
}
