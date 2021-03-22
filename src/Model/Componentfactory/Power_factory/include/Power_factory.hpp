#pragma once

#include "Componentfactory.hpp"

namespace Model::Componentfactory {

  /**
   * @brief add the Power Components to the provided Componentfactory
   * 
   * @param factory to which to add the Power Components
   */
  void add_power_components(Componentfactory &factory);


  struct Power_factory: public Componentfactory{
    Power_factory();
  };
}
