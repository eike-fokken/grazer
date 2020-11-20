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

  std::shared_ptr<Model::Problem>
  setup_problem(std::filesystem::path const &topology,
                std::filesystem::path const &boundary,
                std::filesystem::path const &output_directory) {

    json topologyjson;
    try {
      std::ifstream jsonfilestream(topology);
      jsonfilestream >> topologyjson;
    } catch (...) {
      gthrow({"Couldn't load topology file: ", topology.string()});
    }
    json boundaryjson;
    try {
      std::ifstream jsonfilestream(boundary);
      jsonfilestream >> boundaryjson;
    } catch (...) {
      gthrow({"Couldn't load boundary value file: ", boundary.string()});
    }

    try {
      std::vector<std::unique_ptr<Network::Node>> nodes;
      std::vector<std::unique_ptr<Network::Edge>> edges;
      auto netptr = construct_network(topologyjson, boundaryjson, nodes, edges);

      auto netprobptr = std::unique_ptr<Model::Subproblem>(
          new Model::Networkproblem::Networkproblem(std::move(netptr)));
      std::shared_ptr<Model::Problem> probptr(
          new Model::Problem(output_directory));
      probptr->add_subproblem(std::move(netprobptr));
      return probptr;
    } catch (Exception &e) {
      std::cout << e.what() << std::endl;
      throw;
    }
  }

  std::unique_ptr<Network::Net>
  construct_network(json &topologyjson, json &boundary_json,
                    std::vector<std::unique_ptr<Network::Node>> &nodes,
                    std::vector<std::unique_ptr<Network::Edge>> &edges) {

    json allpowernodes;
    for (auto &node : topologyjson["nodes"]["Vphi_nodes"]) {
      node["type"] = "Vphi";
      allpowernodes.push_back(node);
    }
    for (auto &node : topologyjson["nodes"]["PV_nodes"]) {
      node["type"] = "PV";
      allpowernodes.push_back(node);
    }
    for (auto &node : topologyjson["nodes"]["PQ_nodes"]) {
      node["type"] = "PQ";
      allpowernodes.push_back(node);
    }

    for (auto &node : allpowernodes) {
      std::string nodeid = node["id"].get<std::string>();
      auto finder = [nodeid](json &x) {
        auto it = x.find("id");
        return it != x.end() and it.value() == nodeid;
      };
      auto bdjson =
          std::find_if(boundary_json["boundarycondition"].begin(),
                       boundary_json["boundarycondition"].end(), finder);

      if (node["type"] == "Vphi") {
        nodes.push_back(std::unique_ptr<Model::Networkproblem::Power::Vphinode>(
            new Model::Networkproblem::Power::Vphinode(node["id"], *bdjson,
                                                       node["G"], node["B"])));
      }
      if (node["type"] == "PV") {
        nodes.push_back(std::unique_ptr<Model::Networkproblem::Power::PVnode>(
            new Model::Networkproblem::Power::PVnode(node["id"], *bdjson,
                                                     node["G"], node["B"])));
      }
      if (node["type"] == "PQ") {
        nodes.push_back(std::unique_ptr<Model::Networkproblem::Power::PQnode>(
            new Model::Networkproblem::Power::PQnode(node["id"], *bdjson,
                                                     node["G"], node["B"])));
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

  void set_initial_values(Eigen::VectorXd &new_state,
                          std::filesystem::path const &initial,
                          std::shared_ptr<Model::Problem> &problem) {
    json initialjson;
    try {
      std::ifstream jsonfilestream(initial);
      jsonfilestream >> initialjson;
    } catch (...) {
      gthrow({"Couldn't load initial value file: ", initial.string()});
    }

    problem->set_initial_values(new_state, initialjson);
  }
} // namespace Jsonreader
