#pragma once

#include "Componentfactory.hpp"

namespace Model::Componentfactory {

  /**
   * @brief add Gas Components to the provided Componentfactory
   * @param factory to which the Gas Component schould be added
   */
  void add_gas_components(Componentfactory &factory);

  /// \brief This class provides a Componentfactory of all Gas components.
  struct Gas_factory : public Componentfactory {
    Gas_factory();
  };
}
