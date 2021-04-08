#pragma once
#include "Edge.hpp"
#include "Node.hpp"
#include <map>
#include <memory>

namespace Model::Networkproblem {
  class Equationcomponent;
}

namespace Model::Componentfactory {

// this macro collection is ugly but hopefully makes for clearer compiler
// errors:
#define STRINGIFY(nonstring) STRINGIFYII(nonstring)
#define STRINGIFYII(nonstring) #nonstring
#define LINE_NUMBER_STRING STRINGIFY(__LINE__)

  struct AbstractComponentType {
    virtual ~AbstractComponentType(){};

    /**
     * @brief get the name of the component used inside the topology.json
     * definition
     * @return std::string
     */
    virtual std::string get_name() const = 0;

    /**
     * @brief Get the json schema describing the necessary properties of a
     * component
     * @return nlohmann::json
     */
    virtual nlohmann::json get_schema() const = 0;

    /**
     * @brief Returns true if a directory for output files needs to be
     * generated.
     * @return true if a directory for output files needs to be generated.
     */
    virtual bool needs_output_file() const = 0;
  };

  struct AbstractNodeType : public AbstractComponentType {
    virtual ~AbstractNodeType(){};

    /**
     * @brief Create an instance of this NodeType
     *
     * @param topology the json snippet describing this Node from the
     * topology.json
     * @return std::unique_ptr<Network::Node>
     */
    virtual std::unique_ptr<Network::Node>
    make_instance(nlohmann::json const &topology) const = 0;
  };

  struct AbstractEdgeType : public AbstractComponentType {
    virtual ~AbstractEdgeType(){};

    /**
     * @brief Create an instance of this EdgeType
     *
     * @param topology the json snippet describing this Edge from the
     * topology.json
     * @param nodes the adjacent nodes
     * @return std::unique_ptr<Network::Edge>
     */
    virtual std::unique_ptr<Network::Edge> make_instance(
        nlohmann::json const &topology,
        std::vector<std::unique_ptr<Network::Node>> &nodes) const = 0;
  };

  template <typename ConcreteNode>
  struct NodeType final : public AbstractNodeType {
    static_assert(
        std::is_base_of_v<Network::Node, ConcreteNode>,
        __FILE__ ":" LINE_NUMBER_STRING
                 ": ConcreteNode must inherit from Network::Node.\n Check "
                 "whether an edge was added to the Nodechooser.");

    std::string get_name() const override { return ConcreteNode::get_type(); };
    nlohmann::json get_schema() const override {
      return ConcreteNode::get_schema();
    };

    bool needs_output_file() const override {
      if constexpr (not std::is_base_of_v<
                        Networkproblem::Equationcomponent, ConcreteNode>) {
        return false;
      } else {
        return ConcreteNode::needs_output_file();
      }
    };

    std::unique_ptr<Network::Node>
    make_instance(nlohmann::json const &topology) const override {
      return std::make_unique<ConcreteNode>(topology);
    };
  };

  template <typename ConcreteEdge>
  struct EdgeType final : public AbstractEdgeType {
    static_assert(
        std::is_base_of_v<Network::Edge, ConcreteEdge>,
        __FILE__ ":" LINE_NUMBER_STRING
                 ": ConcreteEdge must inherit from Network::Edge.\n Check "
                 "whether a node was added to the Edgechooser.");

    std::string get_name() const override { return ConcreteEdge::get_type(); };
    nlohmann::json get_schema() const override {
      return ConcreteEdge::get_schema();
    };

    std::unique_ptr<Network::Edge> make_instance(
        nlohmann::json const &topology,
        std::vector<std::unique_ptr<Network::Node>> &nodes) const override {
      return std::make_unique<ConcreteEdge>(topology, nodes);
    };

    bool needs_output_file() const override {
      if constexpr (not std::is_base_of_v<
                        Networkproblem::Equationcomponent, ConcreteEdge>) {
        return false;
      } else {
        return ConcreteEdge::needs_output_file();
      }
    };
  };

  struct Componentfactory {
  protected:
    ~Componentfactory() = default;

  public:
    /// @brief A map of Node types.
    ///
    /// The keys in this map are names of the node types as given in the
    /// topology json. The values are factory objects that can construct a node
    /// of the specific type.
    std::map<std::string, std::unique_ptr<AbstractNodeType>> node_type_map;

    /// @brief A map of Edge types.
    ///
    /// The keys in this map are names of the edge types as given in the
    /// topology json. The values are factory objects that can construct a edge
    /// of the specific type.
    std::map<std::string, std::unique_ptr<AbstractEdgeType>> edge_type_map;

    void add_node_type(std::unique_ptr<AbstractNodeType> nodeType);
    void add_edge_type(std::unique_ptr<AbstractEdgeType> edgeType);

    nlohmann::json get_topology_schema();
  };

} // namespace Model::Componentfactory
