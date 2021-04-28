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
 * The single parameter is the topology file from which to create the graph.
 * @param topology.json The topology file. Should contain Sources, Sinks and/or
 * Innodes.
 */

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cout
        << "You must provide a topology file and a latex filename for printout!"
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

  std::vector<std::string> nodetypes{"Source", "Sink", "Innode"};
  std::vector<std::string> edgetypes
      = {"Pipe", "Controlvalve", "Compressorstation", "Shortpipe"};

  std::ofstream outstream((fs::path(texfile)));

  outstream << "\\documentclass{standalone}\n"
            << "\\usepackage{tikz}\n"
            << "\\begin{document}\n"
            << "\\begin{tikzpicture}[every node"
            << "/.style={scale=0.7,transform shape,circle}]\n";

  for (auto const &type : nodetypes) {
    if (not topology["nodes"].contains(type)) {
      continue;
    }
    auto color = colorchooser(type);
    for (auto const &node : topology["nodes"][type]) {
      std::string id = node["id"];
      auto label_id
          = std::regex_replace(id, std::regex("_"), "\\textunderscore ");
      double x = node["x"];
      x = x / 3;
      double y = node["y"];
      y = y / 3;
      outstream << "\\node[draw=" << color << "](" << id << ") at(" << x << ", "
                << y << "){" << label_id << "};\n";
    }
  }

  for (auto const &type : edgetypes) {
    if (not topology["connections"].contains(type)) {
      continue;
    }
    for (auto const &edge : topology["connections"][type]) {
      std::string from = edge["from"];
      std::string to = edge["to"];

      outstream << "\\draw(" << from << ")--(" << to << ");\n";
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
  //       outputstream << input.dump(4);
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
    return "blue";
  } else {
    throw std::runtime_error(
        "Unknown node type! Did you extend this program without extending the "
        "colorchooser?");
  }
}
