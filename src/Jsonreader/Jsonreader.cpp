#include "Networkproblem.hpp"
#include <Edge.hpp>
#include <Exception.hpp>
#include <Jsonreader.hpp>
#include <Net.hpp>
#include <Node.hpp>
#include <PQnode.hpp>
#include <PVnode.hpp>
#include <Problem.hpp>
#include <Subproblem.hpp>
#include <Transmissionline.hpp>
#include <Vphinode.hpp>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>

namespace Jsonreader {

  using json = nlohmann::ordered_json;

  std::unique_ptr<Model::Problem>
  setup_problem(std::filesystem::path topology, std::filesystem::path initial,
                std::filesystem::path boundary) {

    json topologyjson;
    try {
      std::ifstream jsonfilestream(topology);
      jsonfilestream >> topologyjson;
    } catch (...) {
      gthrow({"Couldn't load topology file: ", topology.string()});
    }
    json initialjson;
    try {
      std::ifstream jsonfilestream(initial);
      jsonfilestream >> initialjson;
    } catch (...) {
      gthrow({"Couldn't load initial value file: ", initial.string()});
    }
    json boundaryjson;
    try {
      std::ifstream jsonfilestream(boundary);
      jsonfilestream >> boundaryjson;
    } catch (...) {
      gthrow({"Couldn't load boundary value file: ", boundary.string()});
    }

    std::map<std::string, std::map<double, Eigen::Vector2d>>
        two_value_boundary_map = get_two_value_boundaries(boundaryjson);
    try {
      std::vector<std::unique_ptr<Network::Node>> nodes;
      std::vector<std::unique_ptr<Network::Edge>> edges;
      auto netptr =
          construct_network(topologyjson, two_value_boundary_map, nodes, edges);

      auto netprobptr = std::unique_ptr<Model::Subproblem>(
          new Model::Networkproblem::Networkproblem(std::move(netptr)));
      std::unique_ptr<Model::Problem> probptr(new Model::Problem);
      probptr->add_subproblem(std::move(netprobptr));
      return probptr;
    } catch (Exception &e) {
      std::cout << e.what() << std::endl;
      throw;
    }
  }

  std::map<std::string, std::map<double, Eigen::Vector2d>>
  get_two_value_boundaries(json boundaryjson) {
    std::map<std::string, std::map<double, Eigen::Vector2d>>
        powerboundaryvalues;
    try {
      for (auto &node : boundaryjson["boundarycondition"]) {
        if (node["type"] == "PQ" or node["type"] == "PV" or
            node["type"] == "Vphi") {
          std::map<double, Eigen::Vector2d> node_boundary;
          for (auto &datapoint : node["data"]) {
            try {

              if (datapoint.size() != 3) {
                gthrow(
                    {"data in node with id ", node["id"], " is not of size 3."})
              };
            } catch (Exception &e) {
              std::cout << e.what() << std::endl;
              throw;
            }
            Eigen::Vector2d value;
            try {
              value[0] = datapoint[1];
              value[1] = datapoint[2];
            } catch (...) {
              gthrow({"data in node with id ", node["id"],
                      " couldn't be assignd in vector, not a double?"})
            }
            node_boundary.insert({datapoint[0], value});
          }
          powerboundaryvalues.insert(
              {node["id"].get<std::string>(), node_boundary});
        } else {
          std::cout << "not implemented yet: boundary values for nodes of type "
                    << node["type"] << " node: " << node["id"] << '\n';
        }
      }
    } catch (...) {

      gthrow({"Failed to read in field \"boundarycondition\"."})
    }
    return powerboundaryvalues;
  }

  std::unique_ptr<Network::Net>
  construct_network(json topologyjson,
                    std::map<std::string, std::map<double, Eigen::Vector2d>>
                        two_value_boundary_map,
                    std::vector<std::unique_ptr<Network::Node>> &nodes,
                    std::vector<std::unique_ptr<Network::Edge>> &edges) {
    for (auto &node : topologyjson["nodes"]["Vphi_nodes"]) {
      auto bd_iterator = two_value_boundary_map.find(node["id"]);
      if (bd_iterator == two_value_boundary_map.end()) {
        gthrow({"The node with ", node["id"],
                " has no boundary condition in the boundary value file."});
      } else {
        // strange as it seems: Here we actually construct a Vphi node:
        nodes.push_back(std::unique_ptr<Model::Networkproblem::Power::Vphinode>(
            new Model::Networkproblem::Power::Vphinode(
                node["id"], (*bd_iterator).second, node["G"], node["B"])));
      }
    }
    for (auto &node : topologyjson["nodes"]["PQ_nodes"]) {
      auto bd_iterator = two_value_boundary_map.find(node["id"]);
      if (bd_iterator == two_value_boundary_map.end()) {
        gthrow({"The node with ", node["id"],
                " has no boundary condition in the boundary value file."});
      } else {
        nodes.push_back(std::unique_ptr<Model::Networkproblem::Power::PQnode>(
            new Model::Networkproblem::Power::PQnode(
                node["id"], (*bd_iterator).second, node["G"], node["B"])));
      }
    }
    for (auto &node : topologyjson["nodes"]["PV_nodes"]) {
      auto bd_iterator = two_value_boundary_map.find(node["id"]);
      if (bd_iterator == two_value_boundary_map.end()) {
        gthrow({"The node with ", node["id"],
                " has no boundary condition in the boundary value file."});
      } else {
        nodes.push_back(std::unique_ptr<Model::Networkproblem::Power::PVnode>(
            new Model::Networkproblem::Power::PVnode(
                node["id"], (*bd_iterator).second, node["G"], node["B"])));
      }
    }

    for (auto &tline : topologyjson["connections"]["transmissionLine"]) {
      // get the pointer from the node vector and put it in the line!

      auto start = std::find_if(nodes.begin(), nodes.end(),
                                [tline](std::unique_ptr<Network::Node> &x) {
                                  auto nodeid = x->get_id();
                                  return nodeid == tline["from"];
                                });
      if (start == nodes.end()) {
        gthrow({"The starting node ", tline["from"],
                ", given by transmission line ", tline["id"],
                ",\n is not defined in the nodes part of the topology file!"});
      }
      auto end = std::find_if(nodes.begin(), nodes.end(),
                              [tline](std::unique_ptr<Network::Node> &x) {
                                auto nodeid = x->get_id();
                                return nodeid == tline["to"];
                              });
      if (end == nodes.end()) {
        gthrow({"The ending node ", tline["to"],
                ", given by transmission line ", tline["id"],
                ",\n is not defined in the nodes part of the topology file!"});
      }
      edges.push_back(
          std::unique_ptr<Model::Networkproblem::Power::Transmissionline>(
              new Model::Networkproblem::Power::Transmissionline(
                  tline["id"], (*start).get(), (*end).get(), tline["G"],
                  tline["B"])));
    }

    std::unique_ptr<Network::Net> net(
        new Network::Net(std::move(nodes), std::move(edges)));
    return net;
  }

} // namespace Jsonreader
