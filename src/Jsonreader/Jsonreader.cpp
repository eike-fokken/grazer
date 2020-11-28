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
#include <Gasboundarynode.hpp>
#include <Innode.hpp>

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
      std::cout << __FILE__ <<":" << __LINE__ << ": Couldn't load topology file: " << topology.string() << std::endl;
      throw;
    }
    json boundaryjson;
    try {
      std::ifstream jsonfilestream(boundary);
      jsonfilestream >> boundaryjson;
    } catch (...) {
      std::cout << __FILE__ << ":" << __LINE__
                << ": Couldn't load boundary file: " << boundary.string()
                << std::endl;
      throw;
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

    std::vector<std::string> powernodetypes({"Vphi", "PV", "PQ"});
    for (auto &powernodetype : powernodetypes) {
      for (auto &powernode : topologyjson["nodes"][powernodetype]) {
        powernode["type"] = powernodetype;
        allpowernodes.push_back(powernode);
      }
    }

    json allgasnodes;

    std::vector<std::string> gasnodetypes({"source", "sink", "innode"});
    for (auto &gasnodetype : gasnodetypes) {
      for (auto &gasnode : topologyjson["nodes"][gasnodetype]) {
        gasnode["type"] = gasnodetype;
        allgasnodes.push_back(gasnode);
      }
    }

    for (auto &gasnode : allgasnodes) {
        std::string nodeid = gasnode["id"].get<std::string>();
        auto finder = [nodeid](json &x) {
          auto it = x.find("id");
          return it != x.end() and it.value() == nodeid;
        };
        auto bdjson =
            std::find_if(boundary_json["boundarycondition"].begin(),
                         boundary_json["boundarycondition"].end(), finder);

        if (gasnode["type"] == "source") {
          nodes.push_back(
              std::unique_ptr<Model::Networkproblem::Gas::Gasboundarynode>(
                  new Model::Networkproblem::Gas::Gasboundarynode(
                      gasnode["id"], *bdjson, gasnode)));
        }

        if (gasnode["type"] == "sink") {
          json sinkboundaryjson = *bdjson;
          for (auto & datapoint : sinkboundaryjson["data"]){
            double value = datapoint["values"][0].get<double>();
            datapoint["values"][0] = -value;
          }
          nodes.push_back(std::unique_ptr<Model::Networkproblem::Gas::Gasboundarynode>
                          (new Model::Networkproblem::Gas::Gasboundarynode(gasnode["id"], sinkboundaryjson, gasnode)));

        }
        if (gasnode["type"] == "innode") {

          nodes.push_back(std::unique_ptr<Model::Networkproblem::Gas::Innode>
                          (new Model::Networkproblem::Gas::Innode(gasnode["id"])));

        }

        // if (gasnode["type"] == "PV") {
        //   // nodes.push_back(std::unique_ptr<Model::Networkproblem::Gas::Source>(
        //   // new Model::Networkproblem::Gas::Source(gasnode["id"], *bdjson,
        //   //                                          gasnode["G"], gasnode["B"])));
        // }
        // if (gasnode["type"] == "PQ") {
        //   // nodes.push_back(std::unique_ptr<Model::Networkproblem::Power::PVnode>(
        //   //                                                                       new Model::Networkproblem::Power::PVnode(
        //   //         gasnode["id"], *bdjson, gasnode["G"], gasnode["B"])));
        // }
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
          nodes.push_back(
              std::unique_ptr<Model::Networkproblem::Power::Vphinode>(
                  new Model::Networkproblem::Power::Vphinode(
                      node["id"], *bdjson, node["G"], node["B"])));
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
          gthrow(
              {"The starting node ", tline["from"],
               ", given by transmission line ", tline["id"],
               ",\n is not defined in the nodes part of the topology file!"});
        }
        auto end = std::find_if(nodes.begin(), nodes.end(),
                                [tline](std::unique_ptr<Network::Node> &x) {
                                  auto nodeid = x->get_id();
                                  return nodeid == tline["to"];
                                });
        if (end == nodes.end()) {
          gthrow(
              {"The ending node ", tline["to"], ", given by transmission line ",
               tline["id"],
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
      std::cout << __FILE__ << ":" << __LINE__
                << ": Couldn't load initial file: " << initial.string()
                << std::endl;
      throw;
    }

    problem->set_initial_values(new_state, initialjson);
  }
} // namespace Jsonreader
