#include "Full_factory.hpp"
#include "schema_generation.hpp"
#include "Input_output.hpp"
#include "Exception.hpp"
#include <filesystem>

using std::filesystem::path;

int main(int argc, char** argv) {
  auto arguments = io::args_as_vector(argc, argv);
  if (arguments.size() == 1) {
    arguments.push_back("schemas"); // default
  } else if (not (arguments.size() == 2)) {
    gthrow({"Unexpected number of arguments"});
  }
  if (not std::filesystem::is_directory(path(arguments[1]))) {
    gthrow({"Argument is not a directory"});
  }
  Model::Componentfactory::Full_factory full_factory;
  Aux::schema::make_schemas(full_factory, arguments[1]);
}