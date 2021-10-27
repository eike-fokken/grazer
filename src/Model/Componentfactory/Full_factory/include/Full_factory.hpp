#pragma once

#include "Componentfactory.hpp"
#include <nlohmann/json.hpp>

namespace Model::Componentfactory {

  /**
   * @brief add all components to the provided Componentfactory
   *
   * @param factory to which the components should be added
   * @param defaults a json with defaults for the individual components
   */
  void
  add_all_components(Componentfactory &factory, nlohmann::json const &defaults);

  /** \brief This class provides a Componentfactory of all Component types
   * defined for the Networkproblem.
   * At the moment these are Gas components and Power components.
   */
  struct Full_factory : public Componentfactory {

    /** \brief Constructs a factory for all known component types of grazer
     *
     * @param defaults a json with defaults for the individual components
     */
    Full_factory(nlohmann::json const &defaults);
  };
} // namespace Model::Componentfactory
