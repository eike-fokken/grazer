#pragma once

#include "Componentfactory.hpp"

namespace Model::Componentfactory {

  /**
   * @brief add all components to the provided Componentfactory
   *
   * @param factory to which the components should be added
   */
  void add_all_components(Componentfactory &factory);

  /// \brief This class provides a Componentfactory of all Component types
  /// defined for the Networkproblem.
  ///
  /// At the moment these are Gas components and Power components.
  struct Full_factory : public Componentfactory {
    Full_factory();
  };
} // namespace Model::Componentfactory
