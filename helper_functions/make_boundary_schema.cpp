#include "Componentfactory.hpp"
#include "Full_factory.hpp"
#include <iostream>

int main() {
  Model::Componentfactory::Full_factory full_factory;
  std::cout << full_factory.get_boundary_schema().dump(/*indent=*/4)
            << std::endl;
}