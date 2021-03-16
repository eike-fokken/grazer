#pragma once
#include "Edge.hpp"
#include "Node.hpp"
#include <map>
#include <memory>

namespace Model::Componentfactory {

  /// \brief This is the type of all factory functions for Nodes in a Networkproblem.
  using Nodefactory =
      std::unique_ptr<Network::Node> (*)(nlohmann::json const &topology);


  /// \brief This is the type of all factory function for Edges in a
  /// Networkproblem.
  using Edgefactory = std::unique_ptr<Network::Edge> (*)(
      nlohmann::json const &topology,
      std::vector<std::unique_ptr<Network::Node>> &nodes);



  /// \brief static interface for Componentfactories
  ///
  /// This is a CRTP interface for Componentfactories
  /// Its sole purpose is to provide maps of factory function pointers.
  template <typename Derived> class Componentfactory_interface {
  protected:
    /// \brief The constructor is protected because this class should only ever be used as a base.
    Componentfactory_interface() {};

  public:
    /// \brief Provides a map of all node factories for the factory.
    ///
    /// @returns a map from nodetype string (e.g. "Source") to factory function pointer.
    std::map<std::string, Nodefactory> get_nodetypemap() {
      return static_cast<Derived*>(this)->get_nodetypemap_impl();
    }

    /// \brief Provides a map of all edge factories for the factory.
    ///
    /// @returns a map from edgetype string (e.g. "Pipe") to factory function pointer.
    std::map<std::string, Edgefactory> get_edgetypemap() {
      return static_cast<Derived*>(this)->get_edgetypemap_impl();
    }
  };



  // this macro collection is ugly but hopefully makes for clearer compiler
// errors:
#define STRINGIFY(nonstring) STRINGIFYII(nonstring)
#define STRINGIFYII(nonstring) #nonstring
#define LINE_NUMBER_STRING STRINGIFY(__LINE__)


  struct Nodedatabuilder_base {
    virtual ~Nodedatabuilder_base(){};

    virtual Nodefactory get_factory() const = 0;

    /// \brief returns the string found in the json data files that identifies
    /// components of type Nodetype.
    virtual std::string get_type() = 0;
  };

  template <typename Nodetype>
  struct Nodedatabuilder final : public Nodedatabuilder_base {
    static_assert(std::is_base_of_v<Network::Node, Nodetype>,
                  __FILE__ ":" LINE_NUMBER_STRING
                           ": Nodetype must inherit from Node.\n Check "
                           "whether an edge was added to the Nodechooser.");

    std::string get_type() override { return Nodetype::get_type(); };

    Nodefactory get_factory() const override {
      return std::make_unique<Nodetype>;
    };
  };

  struct Edgedatabuilder_base {
    virtual ~Edgedatabuilder_base(){};

    /// \brief builds a struct containing a bool stating whether Componenttype
    /// needs boundary conditions and a pointer to a factory function for
    /// Componenttype.
    virtual Edgefactory get_factory() const = 0;
    
    /// \brief returns the string found in the json data files that identifies
    /// components of type Edgetype.
    virtual std::string get_type() = 0;

  };

  template <typename Edgetype>
  struct Edgedatabuilder final : public Edgedatabuilder_base {
    static_assert(std::is_base_of_v<Network::Edge, Edgetype>,
                  __FILE__ ":" LINE_NUMBER_STRING
                           ": Edgetype must inherit from Edge.\n Check "
                           "whether a node was added to the Edgechooser.");

    std::string get_type() override { return Edgetype::get_type(); };

    Edgefactory get_factory() const override {
      return std::make_unique<Edgetype>;
    };
  };


}// namespace Model::Componentfactory
