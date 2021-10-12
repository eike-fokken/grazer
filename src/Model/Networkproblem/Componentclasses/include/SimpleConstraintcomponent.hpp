#pragma once
#include "Constraintcomponent.hpp"
namespace Model::Networkproblem {
  class SimpleConstraintcomponent : public Constraintcomponent {
  public:
    int set_constraint_indices(int next_free_index) final;

  private:
    /// \brief Returns number of constraint variables needed by this component.
    ///
    /// Often this will be implemented by a function returning a literal
    /// int like 2. But for PDES its value is only known after construction.
    ///
    /// @returns number of constraint variables needed by this component
    virtual int needed_number_of_inequalities_per_time_point() const = 0;
  };
} // namespace Model::Networkproblem
