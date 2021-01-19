#include <Edge.hpp>
#include <Exception.hpp>
#include <Node.hpp>
#include <memory>
#include <string>
namespace Network {

  Edge::Edge(std::string _id, Node *start_node, Node *end_node)
      : Idobject(_id), starting_node(start_node), ending_node(end_node) {
    if (starting_node == ending_node) {
      gthrow({"cant create an edge from a node to itself!"});
    }
    starting_node->attach_starting_edge(this);
    ending_node->attach_ending_edge(this);
  }

  Edge::Edge(nlohmann::json const &edge_json,
             std::vector<std::unique_ptr<Node>> &nodes)
      : Idobject(edge_json["id"].get<std::string>()),
        starting_node(get_node_from_json(1,edge_json, nodes)),
        ending_node(get_node_from_json(-1,edge_json, nodes)) {
    if (starting_node == ending_node) {
      gthrow({"cant create an edge from a node to itself!"});
    }
    starting_node->attach_starting_edge(this);
    ending_node->attach_ending_edge(this);
  }

  Node *Edge::get_starting_node() const { return starting_node; }

  Node *Edge::get_ending_node() const { return ending_node; }

  Node *Edge::get_node_from_json(int direction, nlohmann::json const &edge_json,
      std::vector<std::unique_ptr<Node>> &nodes) {
    std::string nodeid;
    if(direction == 1){nodeid = edge_json["from"].get<std::string>();}
    else if (direction == -1) {
      nodeid = edge_json["to"].get<std::string>();
    } else {
      gthrow({"direction must be +/- 1 but was ", std::to_string(direction), " in edge with id ", edge_json["id"].get<std::string>()});
    }

    auto node_itr = std::find_if(nodes.begin(), nodes.end(),
                              [nodeid](std::unique_ptr<Network::Node> &x) {
                                auto candidate_nodeid = x->get_id();
                                return nodeid == candidate_nodeid;
                              });
    if (node_itr == nodes.end()) {
      if(direction == 1){
      gthrow({"The starting node ", nodeid,
              ", given by transmission line ", edge_json["id"],
              ",\n is not defined in the nodes part of the topology ",
              "file!"});
      }
      else {
        gthrow({"The ending node ", nodeid, ", given by transmission line ",
                edge_json["id"],
                ",\n is not defined in the nodes part of the topology ",
                "file!"});
      }
    } else {
      return (*node_itr).get();
    }
  }

} // namespace Network
