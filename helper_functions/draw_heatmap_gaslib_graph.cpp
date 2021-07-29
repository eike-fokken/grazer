
#include "Netfactory.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <stdexcept>
#include <vector>
using json = nlohmann::json;
namespace fs = std::filesystem;

std::pair<double, double> extract_min_max_values(
    json const &results, std::vector<std::string> const &types);

struct rgb {
  int r;
  int g;
  int b;
};

rgb rgb_color(double minimum, double maximum, double value);

std::string colorchooser(std::string const &type);

/** @brief A helper function to write out a latex file which draws a graph of
 * the gaslib part of a topology file.
 *
 * The single parameter is the topology file from which to create the graph.
 * @param topology.json The topology file. Should contain Sources, Sinks
 * and/or Innodes.
 */

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cout << "You must provide a topology file, an outputfile and a latex "
                 "filename for "
                 "printout!"
              << std::endl;
    return 1;
  }
  std::string topfile = argv[1];
  std::string resultsfile = argv[2];
  std::string texfile = argv[3];

  json topology;
  {
    std::ifstream inputstream((fs::path(topfile)));
    inputstream >> topology;
  }

  json results;
  {
    std::ifstream resultsstream(resultsfile);
    resultsstream >> results;
  }

  std::vector<std::string> edgetypes
      = {"Pipe", "Controlvalve", "Compressorstation", "Shortpipe"};

  auto [min, max] = extract_min_max_values(results, edgetypes);
  std::cout << "min:" << min << std::endl;
  std::cout << "max:" << max << std::endl;

  return 0;

  // Model::Networkproblem::insert_second_json_in_topology_json(
  //     topology, results, "data");

  // double divideby = 1.0;
  // if (argc == 3) {
  //   divideby = 10;
  // } else {
  //   divideby = std::stod(argv[3]);
  // }

  // std::vector<std::string> nodetypes{"Source", "Sink", "Innode"};
  // // std::vector<std::string> edgetypes
  // //     = {"Pipe", "Controlvalve", "Compressorstation", "Shortpipe"};

  // std::ofstream outstream((fs::path(texfile)));

  // outstream << "\\documentclass{standalone}\n"
  //           << "\\usepackage{tikz}\n"
  //           << "\\begin{document}\n"
  //           << "\\begin{tikzpicture}[every node"
  //           << "/.style={scale=3,transform shape}]\n";

  // struct Node {
  //   std::string color;
  //   std::string id;
  //   double x;
  //   double y;
  // };
  // std::vector<Node> nodes;
  // for (auto const &type : nodetypes) {
  //   if (not topology["nodes"].contains(type)) {
  //     continue;
  //   }
  //   auto color = colorchooser(type);
  //   for (auto const &node : topology["nodes"][type]) {
  //     std::string id = node["id"];
  //     double x = node["x"].get<double>();
  //     double y = node["y"].get<double>();
  //     nodes.push_back(Node{color, id, x, y});
  //   }
  // }

  // double xmean = 0;
  // double ymean = 0;

  // for (auto &node : nodes) {
  //   xmean += node.x;
  //   ymean += node.y;
  // }
  // xmean = xmean / static_cast<int>(nodes.size());
  // ymean = ymean / static_cast<int>(nodes.size());

  // for (auto &node : nodes) {
  //   auto label_id
  //       = std::regex_replace(node.id, std::regex("_"), "\\textunderscore ");
  //   double out_x = (node.x - xmean) / divideby;
  //   double out_y = (node.y - ymean) / divideby;
  //   outstream << "\\node[draw=" << node.color << "](" << node.id << ") at("
  //             << out_x << ", " << out_y << "){" << label_id << "};\n";
  // }

  // for (auto const &type : edgetypes) {
  //   if (not topology["connections"].contains(type)) {
  //     continue;
  //   }
  //   for (auto const &edge : topology["connections"][type]) {
  //     std::string from = edge["from"];
  //     std::string to = edge["to"];

  //     outstream << "\\draw(" << from << ")--(" << to << ");\n";
  //   }
  // }

  // outstream << "\\end{tikzpicture}\n"
  //           << "\\end{document}\n";
}

std::string colorchooser(std::string const &type) {
  if (type == "Source") {
    return "green";
  } else if (type == "Sink") {
    return "red";
  } else if (type == "Innode") {
    return "black";
  } else {
    throw std::runtime_error(
        "Unknown node type! Did you extend this program without extending "
        "the "
        "colorchooser?");
  }
}

rgb rgb_color(double minimum, double maximum, double value) {
  double ratio = 2 * (value - minimum) / (maximum - minimum);
  int b = static_cast<int>(std::max(0., 255 * (1. - ratio)));
  int r = static_cast<int>(std::max(0., 255 * (ratio - 1)));
  int g = 255 - b - r;
  return {r, g, b};
}

std::pair<double, double> extract_min_max_values(
    json const &results, std::vector<std::string> const &types) {
  double min;
  double max;

  bool initialized = false;

  for (auto const &type : types) {
    if (not results["connections"].contains(type)) {
      continue;
    }
    if (not initialized) {
      min = results["connections"][type][0]["data"][0]["pressure"][0]["value"];
      max = results["connections"][type][0]["data"][0]["pressure"][0]["value"];
      initialized = true;
    }
    for (auto const &component : results["connections"][type]) {
      for (auto &step : component["data"]) {
        for (auto const &[key, quantity] : step.items()) {
          if (key != "pressure") {
            continue;
          }
          for (auto const &xpoint : quantity) {
            if (xpoint["value"] < min) {
              min = xpoint["value"];
            }
            if (xpoint["value"] > max) {
              max = xpoint["value"];
            }
          }
        }
      }
    }
  }
  return {min, max};
}
