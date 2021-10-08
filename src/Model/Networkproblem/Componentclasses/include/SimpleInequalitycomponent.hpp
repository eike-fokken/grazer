#pragma once
#include "Inequalitycomponent.hpp"
namespace Model::Networkproblem {
  class SimpleInequalitycomponent : public Inequalitycomponent {
  public:
    int set_inequality_indices(int next_free_index) final;

  private:
    /// \brief Returns number of inequality variables needed by this component.
    ///
    /// Often this will be implemented by a function returning a literal
    /// int like 2. But for PDES its value is only known after construction.
    ///
    /// @returns number of inequality variables needed by this component
    virtual int get_number_of_inequalities() const = 0;
  };
} // namespace Model::Networkproblem
