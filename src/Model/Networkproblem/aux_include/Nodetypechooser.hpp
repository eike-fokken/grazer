#include "Node.hpp"
#include "nlohmann/json.hpp"
#include <iostream>
#include <type_traits>

namespace Model::Networkproblem::Netprob_Aux {


  struct Nodetypechooser {

    bool operator<(Nodetypechooser const &rhs) {
      return (get_type() < rhs.get_type());
    };

    virtual ~Nodetypechooser(){};
    virtual std::string get_type() const = 0;
    virtual bool needs_boundary_values() const = 0;
    virtual std::unique_ptr<Network::Node>
    build_specific_node(nlohmann::json const &topology) const = 0;
  };


  // this macro collection is ugly but hopefully makes for clearer compiler errors:
#define STRINGIFY(nonstring) STRINGIFYII(nonstring)
#define STRINGIFYII(nonstring) #nonstring
#define LINE_NUMBER_STRING STRINGIFY(__LINE__)

template <typename Nodetype> struct Specific_node_type_chooser :public Nodetypechooser {
  static_assert(std::is_base_of<Network::Node, Nodetype>::value,
                __FILE__ ":" LINE_NUMBER_STRING
                         ": Nodetype must inherit from Network::Node!");

  std::string get_type() const final { return Nodetype::get_type(); };

  bool needs_boundary_values() const final {
    return Nodetype::needs_boundary_values(); };

    /// \brief Calls the relevant Node constructor
    ///
    /// @returns a unique pointer cast to Node.
    // template <typename Nodetype>
    std::unique_ptr<Network::Node>
    build_specific_node(nlohmann::json const &topology) const final {
      return std::make_unique<Nodetype>(Nodetype(topology));
    };

  };

} // namespace Model::Networkproblem::Netprob_Aux
