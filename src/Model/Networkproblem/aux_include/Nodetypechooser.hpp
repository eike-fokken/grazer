#include "Edge.hpp"
#include "nlohmann/json.hpp"
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Model::Networkproblem::Netprob_Aux {

  using Nodefactory = std::unique_ptr<Network::Node> (*)(nlohmann::json const &topology);


  struct Nodedata {
  public:
    Nodedata(bool needs_boundary, Nodefactory nodefactory)
      : needs_boundary_values(needs_boundary),Constructor(nodefactory) {};

    bool const needs_boundary_values;
    // Factory Constructor() const { return data.second; };

    Nodefactory const Constructor;
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



  struct Nodedatabuilder_base {
    virtual ~Nodedatabuilder_base() {};

    /// \brief builds a struct containing a bool stating whether Componenttype needs
    /// boundary conditions and a pointer to a factory function for Componenttype.
    Nodedata build_data() const {
      return Nodedata(type_needs_boundary_values(), constructer_pointer());
    };

    /// \brief returns the string found in the json data files that identifies components of type Nodetype.
    virtual std::string get_type() = 0;

  private:
    /// \brief Returns, whether Nodetype needs boundary values.
    virtual bool type_needs_boundary_values() const = 0;

    /// Returns a function pointer to a factory of Nodetype.
    virtual Nodefactory constructer_pointer() const = 0;
  };

  // this macro collection is ugly but hopefully makes for clearer compiler
  // errors:
#define STRINGIFY(nonstring) STRINGIFYII(nonstring)
#define STRINGIFYII(nonstring) #nonstring
#define LINE_NUMBER_STRING STRINGIFY(__LINE__)

  template <typename Nodetype>
  struct Nodedatabuilder final : public Nodedatabuilder_base {
    static_assert(
                  std::is_base_of_v<Network::Node, Nodetype>,
        __FILE__ ":" LINE_NUMBER_STRING
                 ": Nodetype must inherit from Node.\n Check "
                 "whether an edge was added to the Nodechooser.");

    std::string get_type() override { return Nodetype::get_type(); };

  private:
    bool type_needs_boundary_values() const override {
      return Nodetype::needs_boundary_values();
    };

      Nodefactory constructer_pointer() const override {
        return build_specific_node<Nodetype>;
    };
  };

  using Edgefactory = std::unique_ptr<Network::Edge> (*)(nlohmann::json const &topology);


  struct Edgedata {
  public:
    Edgedata(bool needs_boundary, Edgefactory edgefactory)
      : needs_boundary_values(needs_boundary),Constructor(edgefactory) {};

    bool const needs_boundary_values;
    // Factory Constructor() const { return data.second; };

    Edgefactory const Constructor;
  };


  /// \brief Calls the relevant Edge constructor
  ///
  /// @returns a unique pointer to a newly constructed edge, which is cast to
  /// Edge to enable polymorphism.
  template <typename Edgetype>
  std::unique_ptr<Network::Edge>
  build_specific_edge(nlohmann::json const &topology) {
        return std::make_unique<Edgetype>(topology);
  };



  struct Edgedatabuilder_base {
    virtual ~Edgedatabuilder_base() {};

    /// \brief builds a struct containing a bool stating whether Componenttype needs
    /// boundary conditions and a pointer to a factory function for Componenttype.
    Edgedata build_data() const {
      return Edgedata(type_needs_boundary_values(), constructer_pointer());
    };

    /// \brief returns the string found in the json data files that identifies components of type Edgetype.
    virtual std::string get_type() = 0;

  private:
    /// \brief Returns, whether Edgetype needs boundary values.
    virtual bool type_needs_boundary_values() const = 0;

    /// Returns a function pointer to a factory of Edgetype.
    virtual Edgefactory constructer_pointer() const = 0;
  };

  // this macro collection is ugly but hopefully makes for clearer compiler
  // errors:
#define STRINGIFY(nonstring) STRINGIFYII(nonstring)
#define STRINGIFYII(nonstring) #nonstring
#define LINE_NUMBER_STRING STRINGIFY(__LINE__)

  template <typename Edgetype>
  struct Edgedatabuilder final : public Edgedatabuilder_base {
    static_assert(
                  std::is_base_of_v<Network::Edge, Edgetype>,
        __FILE__ ":" LINE_NUMBER_STRING
                 ": Edgetype must inherit from Edge.\n Check "
                 "whether a node was added to the Edgechooser.");

    std::string get_type() override { return Edgetype::get_type(); };

  private:
    bool type_needs_boundary_values() const override {
      return Edgetype::needs_boundary_values();
    };

      Edgefactory constructer_pointer() const override {
        return build_specific_edge<Edgetype>;
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
