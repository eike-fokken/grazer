#pragma once

#include "Componentfactory.hpp"

namespace Model::Componentfactory {

  /**
   * @brief add the Power Components to the provided Componentfactory
   *
   * @param factory to which to add the Power Components
   * @param defaults a json with defaults for the individual components
   */
  void add_power_components(
      Componentfactory &factory, nlohmann::json const defaults);

  /// \brief This class provides a Componentfactory of all Power components.
  /// \param defaults a json with defaults for the individual components
  struct Power_factory : public Componentfactory {
    Power_factory(nlohmann::json const defaults);
  };
} // namespace Model::Componentfactory
