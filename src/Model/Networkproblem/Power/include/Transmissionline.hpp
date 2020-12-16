#pragma once
#include "Powernode.hpp"
#include <Edge.hpp>
#include <string>

namespace Model::Networkproblem::Power {

  class Transmissionline final : public Network::Edge {

  public:
    Transmissionline(std::string _id, Network::Node *start, Network::Node *end,
                     double _G, double _B);
        

    double get_G() const;
    double get_B() const;

    void display() const override;

    /// Returns a pointer to the starting powernode.
    /// The constructor makes sure that this cast is valid.
    Powernode * get_starting_powernode() const;

    /// Returns a pointer to the ending powernode.
    /// The constructor makes sure that this cast is valid.
    Powernode * get_ending_powernode() const;


    
  private:
    double G;
    double B;
  };

} // namespace Model::Networkproblem::Power
