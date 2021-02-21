#include <Idobject.hpp>
#include <iostream>

namespace Network {

  Idobject::Idobject(std::string const & _id)
      : idptr(std::make_unique<std::string>(_id)) {}

  std::string Idobject::get_id() const { return (*idptr); }


} // namespace Network
