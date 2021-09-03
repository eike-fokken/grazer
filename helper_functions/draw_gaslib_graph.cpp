#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <stdexcept>
#include <vector>
using json = nlohmann::ordered_json;
namespace fs = std::filesystem;

static std::string colorchooser(std::string const &type);

/** @brief A helper function to write out a latex file which draws a graph of
 * the gaslib part of a topology file.
 *
 * @param topology.json The topology file. Should contain Sources, Sinks and/or
 * Innodes.
 * @param tex_file Filename of the texfile to be written. Careful: Files are
 * silently overwritten.
 * @param scale_factor Optional parameter, defaults to 10. Possibly needed
 * because tex cannot work with dimensions larger than some value often attained
 * with large structures like the gaslib data. Higher value means smaller
 * picture.
 *
 * CAREFUL: The labels are modified in here. Search the src for "CAREFUL".
 */

int main(int argc, char **argv) {
  if (argc != 3 and argc != 4) {
    std::cout
        << "You must provide a topology file and a latex filename for printout!"
        << std::endl;
    return 1;
  }
  std::string topfile = argv[1];
  std::string texfile = argv[2];

  double divideby = 1.0;
  if (argc == 3) {
    divideby = 10;
  } else {
    divideby = std::stod(argv[3]);
  }

  json topology;
  {
    std::ifstream inputstream((fs::path(topfile)));
    inputstream >> topology;
  }

  std::vector<std::string> nodetypes{"Source", "Sink", "Innode"};
  std::vector<std::string> edgetypes
      = {"Pipe", "Controlvalve", "Compressorstation", "Shortpipe"};

  std::ofstream outstream((fs::path(texfile)));

  outstream << "\\documentclass{standalone}\n"
            << "\\usepackage{tikz}\n"
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
      std::string id = node["id"];
      double x = node["x"].get<double>();
      double y = node["y"].get<double>();
      nodes.push_back(Node{color, id, x, y});
    }
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
    // CAREFUL: This should remove the "node_" part of the label but is not
    // suitable for any other type of label!
    label_id.erase(0, 5);
    // = std::regex_replace(node.id, std::regex("_"), "\\textunderscore ");
    double out_x = (node.x - xmean) / divideby;
    double out_y = (node.y - ymean) / divideby;
    outstream << "\\node[line width=4pt,draw=" << node.color << "](" << node.id
              << ") at(" << out_x << ", " << out_y << "){" << label_id
              << "};\n";
  }

  for (auto const &type : edgetypes) {
    if (not topology["connections"].contains(type)) {
      continue;
    }
    for (auto const &edge : topology["connections"][type]) {
      std::string from = edge["from"];
      std::string to = edge["to"];

      outstream << "\\draw[line width=6pt](" << from << ")--(" << to << ");\n";
    }
  }

  outstream << "\\end{tikzpicture}\n"
            << "\\end{document}\n";

  //   for (auto &comptype : input) { // nodes and connections
  //     if (not comptype.contains(from_category)) {
  //       continue;
  //     }
  //     // define an equals function as a lambda
  //     auto id_equals = [id](nlohmann::json const &a) -> bool {
  //       return a["id"].get<std::string>() == id;
  //     };
  //     auto &currcat = comptype[from_category];
  //     auto it = std::find_if(currcat.begin(), currcat.end(), id_equals);
  //     if (it == currcat.end()) {
  //       continue;
  //     } else {
  //       comptype[to_category].push_back(*it);
  //       currcat.erase(it);
  //       found = true;
  //     }
  //   }
  //   if (not found) {
  //     std::cout << "No object with id " << id << " in category " <<
  //     from_category
  //               << " in file " << file << std::endl;
  //   } else {
  //     {
  //       std::ofstream outputstream((fs::path(file)));
  //       outputstream << input.dump(1,'\t');
  //     }
  //   }
  // }
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
        "Unknown node type! Did you extend this program without extending the "
        "colorchooser?");
  }
}
