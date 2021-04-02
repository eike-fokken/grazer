#include "Componentfactory.hpp"
#include "Full_factory.hpp"
#include <iostream>

int main() {
  Model::Componentfactory::Full_factory full_factory;
  std::cout << full_factory.get_topology_schema().dump(4) << std::endl;
}