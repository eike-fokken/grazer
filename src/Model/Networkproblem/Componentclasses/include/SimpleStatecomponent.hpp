#pragma once
#include "Statecomponent.hpp"
namespace Model::Networkproblem {
  class SimpleStatecomponent : public Statecomponent {
  public:
    int set_state_indices(int next_free_index) final;

  private:
    /// \brief Returns number of state variables needed by this component.
    ///
    /// Often this will be implemented by a function returning a literal
    /// int like 2. But for PDES its value is only known after construction.
    ///
    /// @returns number of state variables needed by this component
    virtual int needed_number_of_states() const = 0;
  };
} // namespace Model::Networkproblem
