#include <Idobject.hpp>
#include <iostream>

namespace Network {

  nlohmann::json Idobject::get_schema() {
    nlohmann::json schema;
    schema["type"] = "object";
    schema["required"] = {"id"};
    schema["properties"]["id"]["type"] = "string";
    return schema;
  }

  Idobject::Idobject(std::string const & _id)
      : idptr(std::make_unique<std::string const >(_id)) {}

  std::string const & Idobject::get_id() const { return (*idptr); }

  std::string Idobject::get_id_copy() const { return (*idptr); }

} // namespace Network
