#include <Nodetypechooser.hpp>
#include <memory>
#include <set>

namespace Model::Networkproblem::Netprob_Aux {

  using Constructor_pointer = std::unique_ptr<Network::Node>(*)(nlohmann::json const &topology);

  class Nodechooser final
  {
  public:
    Nodechooser();

    std::map<std::string, Nodetypedata> get_map();

    private:
    std::map<std::string,Nodetypedata> data;
    };
}
