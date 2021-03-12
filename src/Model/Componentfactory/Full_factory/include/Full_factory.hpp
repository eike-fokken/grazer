#pragma once

#include "Componentfactory.hpp"

namespace Model::Componentfactory {

  /// \brief Provides factories for all types of Networkcomponents defined in grazer.
  struct Full_factory : public Componentfactory_interface<Full_factory> {

    std::map<std::string, Nodefactory> get_nodetypemap_impl();
    std::map<std::string, Edgefactory> get_edgetypemap_impl();

  };
}
