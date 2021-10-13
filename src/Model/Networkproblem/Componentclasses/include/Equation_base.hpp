#pragma once
#include <nlohmann/json.hpp>
#include <optional>
namespace Model {

  /** Base class for Equationcomponent and Controlcomponent
   *
   * Defines a setup function for any post construction intialization and the
   * possibility to define a boundary schema.
   */

  class Equation_base {
  public:
    virtual ~Equation_base() {}
    /** \brief Utility for setup of things that cannot be done during
     * construction.
     *
     * is called by Networkproblems reserve_state_indices.
     * Usually does nothing, but for example gas nodes
     * claim indices from their attached gas edges.
     */
    virtual void setup();
    static std::optional<nlohmann::json> get_boundary_schema();
  };

} // namespace Model
