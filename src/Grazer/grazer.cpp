#include <Jsonreader.hpp>
#include <Problem.hpp>
#include <iostream>
#include <memory>

int main() {

  auto p = Jsonreader::setup_problem("data/topology_pretty.json",
                                     "data/boundary_pretty.json",
                                     "data/boundary_pretty.json");
  std::cout << "success" << std::endl;

  p->display();
}
