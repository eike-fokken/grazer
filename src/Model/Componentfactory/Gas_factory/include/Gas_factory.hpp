#pragma once

#include "Componentfactory.hpp"
#include <nlohmann/json.hpp>

namespace Model::Componentfactory {

  /**
   * @brief add Gas Components to the provided Componentfactory
   * @param factory to which the Gas Component schould be added
   * @param defaults a json with defaults for the individual components
   */
  void
  add_gas_components(Componentfactory &factory, nlohmann::json const defaults);

  /// \brief This class provides a Componentfactory of all Gas components.
  /// \param defaults a json with defaults for the individual components
  struct Gas_factory : public Componentfactory {
    Gas_factory(nlohmann::json const defaults);
  };
} // namespace Model::Componentfactory
