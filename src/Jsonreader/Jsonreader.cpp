#include "Gaspowerconnection.hpp"
#include "Networkproblem.hpp"
#include <Edge.hpp>
#include <Exception.hpp>
#include <Flowboundarynode.hpp>
#include <Innode.hpp>
#include <Jsonreader.hpp>
#include <Net.hpp>
#include <Node.hpp>
#include <PQnode.hpp>
#include <PVnode.hpp>
#include <Pipe.hpp>
#include <Shortpipe.hpp>
#include <Compressorstation.hpp>
#include <Controlvalve.hpp>
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

    // Here pressure boundary conditions are missing:
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

        if (gasnode["type"] == "source") {
          auto bdjson =
              std::find_if(boundary_json["boundarycondition"].begin(),
                           boundary_json["boundarycondition"].end(), finder);

          nodes.push_back(
              std::unique_ptr<Model::Networkproblem::Gas::Flowboundarynode>(
                  new Model::Networkproblem::Gas::Flowboundarynode(
                      gasnode["id"], *bdjson, gasnode)));
        }

        if (gasnode["type"] == "sink") {
          auto bdjson =
              std::find_if(boundary_json["boundarycondition"].begin(),
                           boundary_json["boundarycondition"].end(), finder);

          json sinkboundaryjson = *bdjson;
          for (auto & datapoint : sinkboundaryjson["data"]){
            double value = datapoint["values"][0].get<double>();
            datapoint["values"][0] = -value;
          }
          nodes.push_back(std::unique_ptr<Model::Networkproblem::Gas::Flowboundarynode>
                          (new Model::Networkproblem::Gas::Flowboundarynode(gasnode["id"], sinkboundaryjson, gasnode)));

        }
        if (gasnode["type"] == "innode") {
          nodes.push_back(std::unique_ptr<Model::Networkproblem::Gas::Innode>
                          (new Model::Networkproblem::Gas::Innode(gasnode["id"])));
        }


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
                  new Model::Networkproblem::Power::Vphinode(      node["id"], *bdjson, std::stod(node["G"].get<std::string>()), std::stod(node["B"].get<std::string>()))));
                                                       
        }
        if (node["type"] == "PV") {
          nodes.push_back(std::unique_ptr<Model::Networkproblem::Power::PVnode>(
              new Model::Networkproblem::Power::PVnode(      node["id"], *bdjson, std::stod(node["G"].get<std::string>()), std::stod(node["B"].get<std::string>()))));
        }
        if (node["type"] == "PQ") {
          nodes.push_back(std::unique_ptr<Model::Networkproblem::Power::PQnode>(
              new Model::Networkproblem::Power::PQnode(      node["id"], *bdjson, std::stod(node["G"].get<std::string>()), std::stod(node["B"].get<std::string>()))));
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
                                                                   tline["id"], (*start).get(), (*end).get(), std::stod(tline["G"].get<std::string>()),
                    std::stod(tline["B"].get<std::string>()))));
      }

      json allgasedges;

      std::vector<std::string> gasedgetype( {"shortPipe", "controlValve", "turboCompressor", "pipe", "gas2power"});
      for(auto & edgetype : gasedgetype) {
        for (auto & gasedge: topologyjson["connections"][edgetype]) {
          gasedge["type"] = edgetype;
          allgasedges.push_back(gasedge);
        }
      }
      
      
      for (auto &edge : allgasedges) {
        std::string edgeid = edge["id"];
        // get the pointer from the node vector and put it in the line

        // check whether the attached nodes are in the network:
        auto start = std::find_if(nodes.begin(), nodes.end(),
                                  [edge](std::unique_ptr<Network::Node> &x) {
                                    auto nodeid = x->get_id();
                                    return nodeid == edge["from"];
                                  });
        if (start == nodes.end()) {
          gthrow(
              {"The starting node ", edge["from"],
               ", given by edge ", edge["id"],
               ",\n is not defined in the nodes part of the topology file!"});
        }
        auto end = std::find_if(nodes.begin(), nodes.end(),
                                [edge](std::unique_ptr<Network::Node> &x) {
                                  auto nodeid = x->get_id();
                                  return nodeid == edge["to"];
                                });
        if (end == nodes.end()) {
          gthrow(
              {"The ending node ", edge["to"], ", given by edge ",
               edge["id"],
               ",\n is not defined in the nodes part of the topology file!"});
        }
        if(edge["type"] == "pipe"){
          edges.push_back(std::unique_ptr<Model::Networkproblem::Gas::Pipe> ( new Model::Networkproblem::Gas::Pipe(edge["id"], (*start).get(), (*end).get(),edge,10000)));
        } else if (edge["type"] == "shortPipe") {
          edges.push_back(std::unique_ptr<Model::Networkproblem::Gas::Shortpipe>(
              new Model::Networkproblem::Gas::Shortpipe(edge["id"], (*start).get(),
                                                   (*end).get())));
        } else if (edge["type"] == "turboCompressor") {
          std::cout << __FILE__<<":" << __LINE__ << ": Careful, Turbocompressor replaced by shortpipe!" <<std::endl;
          edges.push_back(
              std::unique_ptr<Model::Networkproblem::Gas::Shortpipe>(
                  new Model::Networkproblem::Gas::Shortpipe(
                      edge["id"], (*start).get(), (*end).get())));
        } else if (edge["type"] == "controlValve") {
          std::cout << __FILE__<<":" << __LINE__ << ": Careful, Controlvalve replaced by shortpipe!" <<std::endl;
          edges.push_back(
              std::unique_ptr<Model::Networkproblem::Gas::Shortpipe>(
                  new Model::Networkproblem::Gas::Shortpipe(
                      edge["id"], (*start).get(), (*end).get())));
        } else if (edge["type"] == "gas2power") {
          try{
          edges.push_back(std::unique_ptr<Model::Networkproblem::Gas::Gaspowerconnection>(
                                                                                 new Model::Networkproblem::Gas::Gaspowerconnection(edge["id"], (*start).get(),
                                                                                                                                    (*end).get(),edge)));
          } catch(...){
            std::cout << __FILE__ << ":" << __LINE__
                      << ": Failure to set up Gaspowerconnection with topology_json: " << edge 
                      << std::endl;
            throw;
          }
        } else {
          gthrow({" unknown gas edge type: ", edge["type"], ", aborting."});
        }

      }




    std::unique_ptr<Network::Net> net(
        new Network::Net(std::move(nodes), std::move(edges)));
    return net;
    }

  void set_initial_values(Eigen::Ref<Eigen::VectorXd>new_state,
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
    try {
      problem->set_initial_values(new_state, initialjson);
    } catch (...) {
      std::cout << __FILE__ << ":" << __LINE__
                << ": failed to set initial values!" << std::endl;
      throw;
    }
  }
} // namespace Jsonreader
