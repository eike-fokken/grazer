#pragma once
#include <Edge.hpp>
#include <memory>
#include <string>

namespace Model::Networkproblem::Power {

  class Transmissionline : public Network::Edge {

  public:
    Transmissionline(std::string name, Network::Node *start, Network::Node *end,
                     double _G, double _B)
        : Edge(name, start, end), G(_G), B(_B){};

    double get_G();
    double get_B();

  private:
    double G;
    double B;
  };

} // namespace Model::Networkproblem::Power
