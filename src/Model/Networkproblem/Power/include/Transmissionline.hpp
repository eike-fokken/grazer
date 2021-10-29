#pragma once
#include "Powernode.hpp"
#include <Edge.hpp>
#include <memory>
#include <string>

namespace Model::Power {

  class Transmissionline final : public Network::Edge {

  public:
    static std::string get_type();
    static nlohmann::json get_schema();

    Transmissionline(
        nlohmann::json const &topology,
        std::vector<std::unique_ptr<Network::Node>> &nodes);

    double get_G() const;
    double get_B() const;

    /**
     * @brief Returns a pointer to the starting powernode. The constructor makes
     * sure that this cast is valid.
     *
     * @return Powernode*
     */
    Powernode *get_starting_powernode() const;

    /**
     * @brief Returns a pointer to the ending powernode. The constructor makes
     * sure that this cast is valid.
     *
     * @return Powernode*
     */
    Powernode *get_ending_powernode() const;

  private:
    double G;
    double B;
  };

} // namespace Model::Power
