#pragma once
#include "Edge.hpp"
#include "Node.hpp"
#include "Nodetypechooser.hpp"
#include <memory>
#include <set>

namespace Model::Networkproblem::Netprob_Aux {

  /// \brief This class constructs a map of nodetypes for constructing nodes from a json file.
  class Nodechooser final {
  public:
    Nodechooser();

    std::map<std::string, Nodedata> get_map();

  private:
    std::map<std::string, Nodedata> data;
  };




  /// \brief This class constructs a map of edgetypes for constructing edges
  /// from a json file.
  class Edgechooser final {
  public:
    Edgechooser();

    std::map<std::string, Edgedata> get_map();

  private:
    std::map<std::string, Edgedata> data;
  };


} // namespace Model::Networkproblem::Netprob_Aux
