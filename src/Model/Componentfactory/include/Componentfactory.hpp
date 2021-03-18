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

  struct AbstractComponentType {
    virtual ~AbstractComponentType(){};

    /**
     * @brief get the name of the component used inside the topology.json definition
     * @return std::string 
     */
    virtual std::string get_name() const = 0;

    /**
     * @brief Get the json schema describing the necessary properties of a component
     * @return nlohmann::json 
     */
    virtual nlohmann::json get_schema() const = 0;
  };

  struct AbstractNodeType : public AbstractComponentType {
    virtual ~AbstractNodeType(){};

    /** deprecated */
    virtual Nodefactory get_factory() const = 0;

    /**
     * @brief Create an instance of this NodeType
     * 
     * @param topology the json snippet describing this Node from the topology.json
     * @return std::unique_ptr<Network::Node> 
     */
    virtual std::unique_ptr<Network::Node> make_instance(
      nlohmann::json const &topology) const = 0;
  };

  struct AbstractEdgeType : public AbstractComponentType {
    virtual ~AbstractEdgeType(){};

    /** deprecated */
    virtual Edgefactory get_factory() const = 0;
    
    /**
     * @brief Create an instance of this EdgeType
     * 
     * @param topology the json snippet describing this Edge from the topology.json
     * @param nodes the adjacent nodes
     * @return std::unique_ptr<Network::Edge> 
     */
    virtual std::unique_ptr<Network::Edge> make_instance(
      nlohmann::json const &topology,
      std::vector<std::unique_ptr<Network::Node>> &nodes
    ) const = 0;
  };

  template <typename Node>
  struct NodeType final : public AbstractNodeType {
    static_assert(std::is_base_of_v<Network::Node, Node>,
                  __FILE__ ":" LINE_NUMBER_STRING
                           ": Node must inherit from Network::Node.\n Check "
                           "whether an edge was added to the Nodechooser.");

    std::string get_name() const override { return Node::get_type(); };
    nlohmann::json get_schema() const override { return Node::get_schema(); };
    std::unique_ptr<Network::Node> make_instance(
      nlohmann::json const &topology
    ) const override {
      return std::make_unique<Node>(topology);
    };

    Nodefactory get_factory() const override {
      return [](
          nlohmann::json const &topology
      ) -> std::unique_ptr<Network::Node> {
        return std::make_unique<Node>(topology);
      };
    };
  };


  template <typename Edge>
  struct EdgeType final : public AbstractEdgeType {
    static_assert(std::is_base_of_v<Network::Edge, Edge>,
                  __FILE__ ":" LINE_NUMBER_STRING
                           ": Edge must inherit from Network::Edge.\n Check "
                           "whether a node was added to the Edgechooser.");

    std::string get_name() const override { return Edge::get_type(); };
    nlohmann::json get_schema() const override { return Edge::get_schema(); };

    std::unique_ptr<Network::Edge> make_instance(
      nlohmann::json const &topology,
      std::vector<std::unique_ptr<Network::Node>> &nodes
    ) const override {
      return std::make_unique<Edge>(topology, nodes);
    };

    Edgefactory get_factory() const override {
      return [](
          nlohmann::json const &topology,
          std::vector<std::unique_ptr<Network::Node>> &nodes
      ) -> std::unique_ptr<Network::Edge> {
        return std::make_unique<Edge>(topology, nodes);
      };
    };
  };

  struct Componentfactory {
    std::map<std::string, std::unique_ptr<AbstractNodeType>> node_type_map;
    std::map<std::string, std::unique_ptr<AbstractEdgeType>> edge_type_map;

    // std::map<std::string, std::unique_ptr<AbstractNodeType>> get_node_type_map();
    // std::map<std::string, std::unique_ptr<AbstractEdgeType>> get_edge_type_map();

    void add_node_type(std::unique_ptr<AbstractNodeType> nodeType);
    void add_edge_type(std::unique_ptr<AbstractEdgeType> edgeType);

    nlohmann::json get_topology_schema();
    
  };


}// namespace Model::Componentfactory
