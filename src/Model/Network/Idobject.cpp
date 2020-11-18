#include <Idobject.hpp>
#include <iostream>

namespace Network {

  std::string Idobject::get_id() const { return id; }

  void Idobject::display() const { std::cout << "id: " << id << "\n"; }

  void Idobject::print_id() const { std::cout << "id: " << id << ", "; }

} // namespace Network
