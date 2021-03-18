#pragma once

#include "Componentfactory.hpp"

namespace Model::Componentfactory {

  /**
   * @brief add all components to the provided Componentfactory
   * 
   * @param factory to which the components should be added
   */
  void add_all_components(Componentfactory &factory);

  /// \brief Provides factories for all types of Networkcomponents defined in grazer.
  struct Full_factory : public Componentfactory_interface<Full_factory> {

    std::map<std::string, Nodefactory> get_nodetypemap_impl();
    std::map<std::string, Edgefactory> get_edgetypemap_impl();

  };
}
