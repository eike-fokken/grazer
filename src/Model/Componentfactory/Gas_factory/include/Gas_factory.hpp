#pragma once

#include "Componentfactory.hpp"

namespace Model::Componentfactory {

  /// \brief Provides factories for gas component types.
  struct Gas_factory : public Componentfactory_interface<Gas_factory> {

    std::map<std::string, Nodefactory> get_nodetypemap_impl();
    std::map<std::string, Edgefactory> get_edgetypemap_impl();

  };
}
