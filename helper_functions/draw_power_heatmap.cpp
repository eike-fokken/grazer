
#include "Netfactory.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <stdexcept>
#include <string>
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
  if (argc != 4 and argc != 5) {
    std::cout << "You must provide a topology file, an outputfile and a latex "
                 "filename for printout! In addition you can provide a scaling "
                 "factor."
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

  Model::Networkproblem::sort_json_vectors_by_id(results, "results");
  Model::Networkproblem::sort_json_vectors_by_id(topology, "topology");

  std::vector<std::string> nodetypes{
      "PQnode", "PVnode", "Vphinode", "StochasticPQnode", "ExternalPowerplant"};
  std::vector<std::string> edgetypes = {"Transmissionline"};

  auto [min, max] = extract_min_max_values(results, nodetypes);
  std::cout << "min:" << min << std::endl;
  std::cout << "max:" << max << std::endl;

  Model::Networkproblem::insert_second_json_in_topology_json(
      topology, results, "results");

  double divideby = 1.0;
  if (argc == 4) {
    divideby = 0.1;
  } else {
    divideby = std::stod(argv[4]);
  }

  std::ofstream outstream((fs::path(texfile)));

  outstream << "\\documentclass{standalone}\n"
            << "\\usepackage{tikz}\n"
            << "\\usepackage{mathptmx}\n"
            << "\\begin{document}\n"
            << "\\begin{tikzpicture}[every node"
            << "/.style={scale=3,transform shape}]\n";

  struct Node {
    std::string color;
    std::string id;
    double x;
    double y;
  };
  std::vector<Node> nodes;
  for (auto const &type : nodetypes) {
    if (not topology["nodes"].contains(type)) {
      continue;
    }
    // auto color = colorchooser(type);
    double value = min;
    for (auto const &node : topology["nodes"][type]) {
      if (not node.contains("results")) {
        std::cout << node.dump(1, '\t');
        throw std::runtime_error("no results!!");
      }
      for (auto const &step : node["results"]["data"]) {
        for (auto const &[key, quantity] : step.items()) {
          if (key != "P") {
            continue;
          }
          if (value < quantity.get<double>()) {
            value = quantity.get<double>();
          }
        }
      }
      std::cout << "value: " << value << std::endl;
      auto rgbcolor = rgb_color(min, max, value);
      auto color = "{rgb:red," + std::to_string(rgbcolor.r) + ";green,"
                   + std::to_string(rgbcolor.g) + ";blue,"
                   + std::to_string(rgbcolor.b) + "}";
      //////////////////////////////////////////
      std::string id = node["id"];
      double x = node["x"].get<double>();
      double y = node["y"].get<double>();
      nodes.push_back(Node{color, id, x, y});
    }

    /////////////////////////////////////////////

    ////////////////////////////////////////////
  }

  double xmean = 0;
  double ymean = 0;

  for (auto &node : nodes) {
    xmean += node.x;
    ymean += node.y;
  }
  xmean = xmean / static_cast<int>(nodes.size());
  ymean = ymean / static_cast<int>(nodes.size());

  double max_x = 0;
  double min_x = 0;
  double max_y = 0;
  double min_y = 0;
  for (auto &node : nodes) {

    auto label_id = node.id;
    // label_id.erase(0, 5);

    // auto label_id
    //     = std::regex_replace(node.id, std::regex("_"), "\\textunderscore ");
    double out_x = (node.x - xmean) / divideby;
    double out_y = (node.y - ymean) / divideby;

    if (out_x > max_x) {
      max_x = out_x;
    }
    if (out_y > max_y) {
      max_y = out_y;
    }
    if (out_y < min_y) {
      min_y = out_y;
    }
    if (out_x < min_x) {
      min_x = out_x;
    }

    outstream << "\\node[draw=" << node.color << ",circle,line width=8pt]("
              << node.id << ") at(" << out_x << ", " << out_y << "){"
              << label_id << "};\n";
  }

  double legend_x = max_x + 0.1 * (max_x - min_x);

  for (int i = 0; i != 101; ++i) {

    // auto color = rgb_color(min, max, value);
    // outstream << "\\node[fill={rgb:red," << color.r << ";green," << color.g
    //           << ";blue," << color.b << "}](myid" << i << ") at(" << legend_x
    //           << ", " << min_y + i * 0.01 * (max_y - min_y) << "){};\n";
    if (i % 10 == 0) {
      double value = min + i * 0.01 * (max - min);
      value = std::round(1000 * value) * 0.001;
      outstream << "\\node[label={[label distance=10pt]right:{ "
                   "\\fontsize{45}{54}\\selectfont"
                << value << "}},inner sep=0pt,minimum size=0pt](myid" << i
                << ") at(" << legend_x << ", "
                << min_y + i * 0.01 * (max_y - min_y) << "){};\n";
    } else {
      outstream << "\\node[inner sep=0pt,minimum size=0pt](myid" << i << ") at("
                << legend_x << ", " << min_y + i * 0.01 * (max_y - min_y)
                << "){};\n";
    }
  }
  for (int i = 0; i != 100; ++i) {
    double value = min + i * 0.01 * (max - min);
    auto color = rgb_color(min, max, value);
    outstream << "\\draw[line width=50pt,color={rgb:red," << color.r
              << ";green," << color.g << ";blue," << color.b << "}](myid" << i
              << ")--(myid" << i + 1 << ");\n";
  }

  for (auto const &type : edgetypes) {
    if (not topology["connections"].contains(type)) {
      continue;
    }
    // bool done = false;
    for (auto const &edge : topology["connections"][type]) {
      std::string from = edge["from"];
      std::string to = edge["to"];
      outstream << "\\draw[line width=5pt](" << from << ")--(" << to << ");\n";
    }
  }

  outstream << "\\end{tikzpicture}\n"
            << "\\end{document}\n";
}

std::string colorchooser(std::string const &type) {
  if (type == "Source") {
    return "green";
  } else if (type == "Sink") {
    return "red";
  } else if (type == "Innode") {
    return "black";
  } else if (type == "Vphinode") {
    return "green";
  } else if (type == "PQnode") {
    return "red";
  } else if (type == "PVnode") {
    return "blue";
  } else if (type == "StochasticPQnode") {
    return "red";
  } else if (type == "ExternalPowerplant") {
    return "green";
  } else {
    throw std::runtime_error(
        "Unknown node type! Did you extend this program without extending "
        "the "
        "colorchooser?");
  }
}

rgb rgb_color(double minimum, double maximum, double value) {
  if (value < minimum or value > maximum) {
    std::cout << "min:" << minimum << std::endl;
    std::cout << "max:" << maximum << std::endl;
    std::cout << "value:" << value << std::endl;
    throw std::runtime_error("A value was not in between minimum and maximum!");
  }
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
    if (not results["nodes"].contains(type)) {
      continue;
    }
    if (not initialized) {
      min = results["nodes"][type][0]["data"][0]["P"];
      max = results["nodes"][type][0]["data"][0]["P"];
      initialized = true;
    }
    for (auto const &component : results["nodes"][type]) {
      for (auto &step : component["data"]) {
        for (auto const &[key, quantity] : step.items()) {
          if (key != "P") {
            continue;
          }
          if (quantity.get<double>() < min) {
            min = quantity.get<double>();
          }
          if (quantity.get<double>() > max) {
            max = quantity.get<double>();
          }
        }
      }
    }
  }
  return {min, max};
}