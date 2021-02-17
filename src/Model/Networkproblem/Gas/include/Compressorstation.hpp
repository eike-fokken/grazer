#pragma once
#include "Shortpipe.hpp"


namespace Model::Networkproblem::Gas {

  class Compressorstation: public Shortpipe {
  public:
    using Shortpipe::Shortpipe;

    static std::string get_type();
    
  };
}
