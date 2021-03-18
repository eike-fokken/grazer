#pragma once

#include "Componentfactory.hpp"

namespace Model::Componentfactory {

  /**
   * @brief add the Power Components to the provided Componentfactory
   * 
   * @param factory to which to add the Power Components
   */
  void add_power_components(Componentfactory &factory);

  struct PowerFactory : public Componentfactory {
    PowerFactory();
  };

  /// \brief Provides factories for power component types.
  struct Power_factory : public Componentfactory_interface<Power_factory> {

    std::map<std::string, Nodefactory> get_nodetypemap_impl();
    std::map<std::string, Edgefactory> get_edgetypemap_impl();

  };
}
