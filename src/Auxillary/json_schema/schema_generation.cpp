#include <Exception.hpp>
#include <fstream>
#include <schema_generation.hpp>

namespace Aux::schema {
  using Model::Componentfactory::Componentfactory;
  using std::filesystem::path;

  // private functions
  bool
  key_from_map_is_prefix(std::map<std::string, json> map, std::string string);

  /**
   * @brief directory validation (directory may only include known schema files)
   * @param directory
   * @param schemas
   */
  void assert_only_known_schemas(
      path directory, std::map<std::string, json> schemas);
  ///////////////////////////////////////////////////////////////////////////

  int make_full_factory_schemas(path grazer_dir){
    Model::Componentfactory::Full_factory full_factory;
    make_schemas(full_factory, grazer_dir / "schemas");
    return 0;
  }

  void make_schemas(Componentfactory const &factory, path schema_dir) {

    std::map<std::string, json> schemas{
        {"topology", factory.get_topology_schema(/*include_external=*/false)},
        {"initial", factory.get_initial_schema()},
        {"boundary", factory.get_boundary_schema()},
        {"control", factory.get_control_schema()}};

    if (std::filesystem::exists(schema_dir)) {
      assert_only_known_schemas(schema_dir, schemas);
    } else {
      std::filesystem::create_directory(schema_dir);
    }

    for (auto const &[name, schema] : schemas) {
      path file = schema_dir / path(name + "_schema.json");
      std::ofstream initial_stream(file);
      initial_stream << factory.get_initial_schema().dump(/*indent=*/2);
    }
  }

  void assert_only_known_schemas(
      path directory, std::map<std::string, json> schemas) {
    for (auto const &file : std::filesystem::directory_iterator(directory)) {
      std::string filename = file.path().filename().string();
      if (std::filesystem::is_directory(file)) {
        gthrow(
            {"unknown directory ", filename, " inside ", directory.string(),
             " aborting to avoid overriding data"});
      }
      if (not key_from_map_is_prefix(schemas, filename)) {
        gthrow(
            {"unknown file ", filename, "inside", directory.string(),
             "are you sure this is the right directory?"});
      }
    }
  }

  bool
  key_from_map_is_prefix(std::map<std::string, json> map, std::string string) {
    for (auto const &[name, _] : map) {
      if (string.rfind(name, 0) == 0) {
        return true;
      }
    }
    return false;
  }

} // namespace Aux::schema