#include "Node.hpp"
#include "nlohmann/json.hpp"
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Model::Networkproblem::Netprob_Aux {


  using Constructor_pointer =
      std::unique_ptr<Network::Node> (*)(nlohmann::json const &topology);

  struct Nodetypedata {
  public:
    Nodetypedata(bool needs_boundary, Constructor_pointer constructor_pointer)
      : needs_boundary_values(needs_boundary),Constructor(constructor_pointer) {};

    bool const needs_boundary_values;
    // Constructor_pointer Constructor() const { return data.second; };

    Constructor_pointer const Constructor;

  };






  template <typename Nodetype>
  bool type_needs_boundary_values() {
    return Nodetype::needs_boundary_values();
  };

  /// \brief Calls the relevant Node constructor
  ///
  /// @returns a unique pointer to a newly constructed node, which is cast to
  /// Node to enable polymorphism.
  template <typename Nodetype>
  std::unique_ptr<Network::Node>
  build_specific_node(nlohmann::json const &topology) {
    return std::make_unique<Nodetype>(topology);
  };



  struct Nodetypedatabuilder_base {

    virtual ~Nodetypedatabuilder_base() {};

    virtual std::string get_type() = 0;
    virtual bool type_needs_boundary_values() = 0;
    virtual Constructor_pointer constructer_pointer() = 0;

  Nodetypedata build_data() {
    return Nodetypedata(type_needs_boundary_values(),constructer_pointer());
    };
  };

  // this macro collection is ugly but hopefully makes for clearer compiler
  // errors:
#define STRINGIFY(nonstring) STRINGIFYII(nonstring)
#define STRINGIFYII(nonstring) #nonstring
#define LINE_NUMBER_STRING STRINGIFY(__LINE__)

  template <typename Nodetype>
  struct Nodetypedatabuilder final: public Nodetypedatabuilder_base {
    static_assert(std::is_base_of<Network::Node, Nodetype>::value,
                  __FILE__ ":" LINE_NUMBER_STRING
                           ": Nodetype must inherit from Network::Node!");

    std::string get_type() override { return Nodetype::get_type(); };
    bool type_needs_boundary_values() override {
      return Nodetype::needs_boundary_values();
    };

    Constructor_pointer constructer_pointer() override {
      return build_specific_node<Nodetype>;
    };

  };



} // namespace Model::Networkproblem::Netprob_Aux

// struct Nodetypechooser {

//   bool operator<(Nodetypechooser const &rhs) {
//     return (get_type() < rhs.get_type());
//   };

//   virtual ~Nodetypechooser(){};
//   virtual std::string get_type() const = 0;
//   virtual bool needs_boundary_values() const = 0;
//   virtual std::unique_ptr<Network::Node>
//   build_specific_node(nlohmann::json const &topology) const = 0;
// };
