#include "Componentfactory.hpp"
#include "Full_factory.hpp"
#include <iostream>

int main() {
  Model::Componentfactory::Full_factory full_factory;
  std::cout << full_factory.get_topology_schema(/*include_external=*/false)
                   .dump(1,'\t')
            << std::endl;
}
