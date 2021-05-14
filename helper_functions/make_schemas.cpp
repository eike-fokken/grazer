#include "Exception.hpp"
#include "Full_factory.hpp"
#include "Input_output.hpp"
#include "schema_generation.hpp"
#include <filesystem>

using std::filesystem::path;
namespace fs = std::filesystem;

int main(int argc, char **argv) {
  auto arguments = io::args_as_deque(argc, argv);

  path schema_dir;
  switch (arguments.size()) {
  case 0:
    schema_dir = path("schema"); // default
    break;
  case 1: schema_dir = path(arguments[1]); break;
  default: gthrow({"Unexpected number of arguments"});
  }
  if (not fs::exists(schema_dir)) {
    fs::create_directory(schema_dir);
  } else if (not fs::is_directory(schema_dir)) {
    gthrow({"Argument is not a directory"});
  }
  Model::Componentfactory::Full_factory full_factory;
  Aux::schema::make_schemas(full_factory, schema_dir);
}