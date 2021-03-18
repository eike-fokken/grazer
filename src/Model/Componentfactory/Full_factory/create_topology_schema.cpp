
#include <Componentfactory.hpp>
#include <Full_factory.hpp>

using Model::Componentfactory::Componentfactory;
using Model::Componentfactory::add_all_components;

int main() {
  Componentfactory full_factory;
  add_all_components(full_factory);
  full_factory.get_topology_schema();  
}