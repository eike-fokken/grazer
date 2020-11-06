#pragma once
#include <Equationcomponent.hpp>

namespace Model::Networkproblem::Power {

class Powernode : public Equationnode {

public:
  virtual ~Powernode(){};

  virtual unsigned int get_number_of_states() override final { return 2; };
};

} // namespace Model::Networkproblem::Power
