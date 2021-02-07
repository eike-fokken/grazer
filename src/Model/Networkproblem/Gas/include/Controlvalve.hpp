#pragma once
#include <Gasedge.hpp>
#include <Shortpipe.hpp>


namespace Model::Networkproblem::Gas {

  class Controlvalve: public Shortpipe {
  public:
    using Shortpipe::Shortpipe;

    static std::string get_type();
    
  };
}
