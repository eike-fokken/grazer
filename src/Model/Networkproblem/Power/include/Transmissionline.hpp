#pragma once
#include "Powernode.hpp"
#include <Edge.hpp>
#include <memory>
#include <string>


namespace Model::Networkproblem::Power {

  class Transmissionline final : public Network::Edge {

  public:

    static std::string get_type();
    static bool needs_boundary_values();

    Transmissionline(nlohmann::json const & topology, std::vector<std::unique_ptr<Network::Node>> & nodes);

    double get_G() const;
    double get_B() const;


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
