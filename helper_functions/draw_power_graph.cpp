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

std::string colorchooser(std::string const &type);

/** @brief A helper function to write out a latex file which draws a graph of
 * the power part of a topology file.
 *
 * @param topology.json The topology file.
 * @param tex_file Filename of the texfile to be written. Careful: Files are
 * silently overwritten.
 * @param scale_factor Optional parameter, defaults to 10. Possibly needed
 * because tex cannot work with dimensions larger than some value often attained
 * with large structures like the gaslib data. Higher value means smaller
 * picture.
 */

int main(int argc, char **argv) {
  if (argc != 3 and argc != 4) {
    std::cout << "You must provide a topology file, an outputfile and a latex "
                 "filename for printout! In addition you can provide a scaling "
                 "factor."
              << std::endl;
    return 1;
  }
  std::string topfile = argv[1];
  std::string texfile = argv[2];

  json topology;
  {
    std::ifstream inputstream((fs::path(topfile)));
    inputstream >> topology;
  }

  std::vector<std::string> nodetypes{
      "PQnode", "PVnode", "Vphinode", "StochasticPQnode", "ExternalPowerplant"};
  std::vector<std::string> edgetypes = {"Transmissionline"};

  double divideby = 1.0;
  if (argc == 3) {
    divideby = 0.1;
  } else {
    divideby = std::stod(argv[3]);
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
    auto color = colorchooser(type);
    for (auto const &node : topology["nodes"][type]) {

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

  for (auto &node : nodes) {

    auto label_id = node.id;
    // label_id.erase(0, 5);

    // auto label_id
    //     = std::regex_replace(node.id, std::regex("_"), "\\textunderscore ");
    double out_x = (node.x - xmean) / divideby;
    double out_y = (node.y - ymean) / divideby;

    outstream << "\\node[draw=" << node.color << ",circle,line width=10pt]("
              << node.id << ") at(" << out_x << ", " << out_y << "){"
              << label_id << "};\n";
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
