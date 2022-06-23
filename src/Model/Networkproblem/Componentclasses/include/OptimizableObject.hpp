#pragma once
#include "Constraintcomponent.hpp"
#include "Controlcomponent.hpp"
#include "Costcomponent.hpp"
#include "Statecomponent.hpp"

namespace Model {

  class OptimizableObject :
      public Statecomponent,
      public Controlcomponent,
      public Constraintcomponent,
      public Costcomponent {};

} // namespace Model
