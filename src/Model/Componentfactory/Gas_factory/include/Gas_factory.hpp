#pragma once

#include "Componentfactory.hpp"

namespace Model::Componentfactory {

  /**
   * @brief add Gas Components to the provided Componentfactory
   * @param factory to which the Gas Component schould be added
   */
  void add_gas_components(Componentfactory &factory);

  /// \brief Provides factories for gas component types.
  struct Gas_factory : public Componentfactory_interface<Gas_factory> {

    std::map<std::string, Nodefactory> get_nodetypemap_impl();
    std::map<std::string, Edgefactory> get_edgetypemap_impl();

  };
}
