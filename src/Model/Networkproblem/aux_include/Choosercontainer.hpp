#include <Nodetypechooser.hpp>
#include <memory>
#include <set>

namespace Model::Networkproblem::Netprob_Aux {

  class Nodechooserset: public std::set<std::unique_ptr<Nodetypechooser> >{
  public:
    Nodechooserset();
  };


}
