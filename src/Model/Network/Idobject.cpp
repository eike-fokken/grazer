#include <Idobject.hpp>
#include <iostream>

namespace Network {

  Idobject::Idobject(std::string const & _id)
      : idptr(std::make_unique<std::string const >(_id)) {}

  std::string const & Idobject::get_id() const { return (*idptr); }

  std::string Idobject::get_id_copy() const { return (*idptr); }

} // namespace Network
