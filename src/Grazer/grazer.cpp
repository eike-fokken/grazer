#include "schema_generation.hpp"
#include "schema_key_insertion.hpp"
#include <filesystem>
#include <iostream>
#include <string>
#include <structopt/structopt.hpp>

#include "commands.hpp"

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
    std::cout << "\n[Error]: " << ex.what() << std::endl;
  } catch (...) {
    std::cout << "An unkown type of exception was thrown.\n\n"
                 "This is a bug and must be fixed!\n\n"
              << std::endl;
  }
  std::cout << app.help() << std::endl;
  return 1;
}
