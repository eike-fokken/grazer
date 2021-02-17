#pragma once
#include "Edge.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <tuple>
#include <type_traits>
#include <utility>

// this macro collection is ugly but hopefully makes for clearer compiler
// errors:
#define STRINGIFY(nonstring) STRINGIFYII(nonstring)
#define STRINGIFYII(nonstring) #nonstring
#define LINE_NUMBER_STRING STRINGIFY(__LINE__)

namespace Model::Networkproblem::Componentfactory {

  using Nodefactory =
      std::unique_ptr<Network::Node> (*)(nlohmann::json const &topology);

  struct Nodedata {
  public:
    Nodedata(Nodefactory nodefactory) : Constructor(nodefactory){};

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
  }

  struct Nodedatabuilder_base {
    virtual ~Nodedatabuilder_base(){};

    /// \brief builds a struct containing a bool stating whether Componenttype
    /// needs boundary conditions and a pointer to a factory function for
    /// Componenttype.
    Nodedata build_data() const { return Nodedata(constructer_pointer()); };

    /// \brief returns the string found in the json data files that identifies
    /// components of type Nodetype.
    virtual std::string get_type() = 0;

  private:
    /// Returns a function pointer to a factory of Nodetype.
    virtual Nodefactory constructer_pointer() const = 0;
  };

  template <typename Nodetype>
  struct Nodedatabuilder final : public Nodedatabuilder_base {
    static_assert(std::is_base_of_v<Network::Node, Nodetype>,
                  __FILE__ ":" LINE_NUMBER_STRING
                           ": Nodetype must inherit from Node.\n Check "
                           "whether an edge was added to the Nodechooser.");

    std::string get_type() override { return Nodetype::get_type(); };

  private:
    Nodefactory constructer_pointer() const override {
      return build_specific_node<Nodetype>;
    };
  };

  using Edgefactory = std::unique_ptr<Network::Edge> (*)(
      nlohmann::json const &topology,
      std::vector<std::unique_ptr<Network::Node>> &nodes);

  struct Edgedata {
  public:
    Edgedata(Edgefactory edgefactory) : Constructor(edgefactory){};

    Edgefactory const Constructor;
  };

  /// \brief Calls the relevant Edge constructor
  ///
  /// @returns a unique pointer to a newly constructed edge, which is cast to
  /// Edge to enable polymorphism.
  template <typename Edgetype>
  std::unique_ptr<Network::Edge>
  build_specific_edge(nlohmann::json const &topology,
                      std::vector<std::unique_ptr<Network::Node>> &nodes) {
    return std::make_unique<Edgetype>(topology, nodes);
  }

  struct Edgedatabuilder_base {
    virtual ~Edgedatabuilder_base(){};

    /// \brief builds a struct containing a bool stating whether Componenttype
    /// needs boundary conditions and a pointer to a factory function for
    /// Componenttype.
    Edgedata build_data() const { return Edgedata(constructer_pointer()); };

    /// \brief returns the string found in the json data files that identifies
    /// components of type Edgetype.
    virtual std::string get_type() = 0;

  private:
    /// Returns a function pointer to a factory of Edgetype.
    virtual Edgefactory constructer_pointer() const = 0;
  };

  template <typename Edgetype>
  struct Edgedatabuilder final : public Edgedatabuilder_base {
    static_assert(std::is_base_of_v<Network::Edge, Edgetype>,
                  __FILE__ ":" LINE_NUMBER_STRING
                           ": Edgetype must inherit from Edge.\n Check "
                           "whether a node was added to the Edgechooser.");

    std::string get_type() override { return Edgetype::get_type(); };

  private:
    Edgefactory constructer_pointer() const override {
      return build_specific_edge<Edgetype>;
    };
  };
} // namespace Model::Networkproblem::Componentfactory
