
#include <Componentfactory.hpp>
#include <iostream>
#include <Full_factory.hpp>

using Model::Componentfactory::Componentfactory;
using Model::Componentfactory::add_all_components;

int main() {
  Componentfactory full_factory;
  add_all_components(full_factory);
  std::cout << full_factory.get_topology_schema().dump() << std::endl;  
}