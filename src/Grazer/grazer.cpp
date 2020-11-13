#include <Problem.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <model.hpp>
#include <nlohmann/json.hpp>
#include <vector>

int main() {

  nlohmann::json netdata;

  std::ifstream jsonfilestream("data/topology.json");

  jsonfilestream >> netdata;

  std::cout << netdata << std::endl;

  //    std::vector<std::unique_ptr<Network::Node>> nodes;

  return 0;
}

// #include <grazer.hpp>

// namespace Grazer {

// int solver() {}

// } // namespace Grazer
