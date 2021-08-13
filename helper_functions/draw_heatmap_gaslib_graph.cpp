
/**
 *\cond
 */

#include "Netfactory.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <nlohmann/json.hpp>
#include <regex>
#include <stdexcept>
#include <vector>

using json = nlohmann::json;
namespace fs = std::filesystem;

struct rgb {
  int r;
  int g;
  int b;
};

/**
 * \endcond
 */

static std::pair<double, double> extract_min_max_values(
    json const &results, std::vector<std::string> const &types);
static rgb rgb_color(double minimum, double maximum, double value);
static std::string colorchooser(std::string const &type);

/** @brief A helper function to write out a latex file which draws a heatmap of
 * a gas net, where the color shows maximal pressures attained over a simulation
 * time frame. (E.g.\ maximal pressure over the curse of 24 hours). Takes the
 * maximal value along each edge (that means an edge is painted in a single
 * color).
 *
 * Ignores Shortpipes and Sinks because at least in Gaslib-134 these are
 * just connected via Shortpipe to a single Innode.
 *
 * @param topology_json_file The topology file.
 * @param results_json_file For every edge must contain results for the
 * key "pressure".
 * @param tex_file Filename of the texfile to be written. Careful: Files are
 * silently overwritten.
 * @param scale_factor Optional parameter, defaults to 10. Possibly needed
 * because tex cannot work with dimensions larger than some value often attained
 * with large structures like the gaslib data. Higher value means smaller
 * picture.
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

  std::vector<std::string> nodetypes{"Source", "Innode"};
  std::vector<std::string> edgetypes
      = {"Pipe", "Controlvalve", "Compressorstation"};

  auto [min, max] = extract_min_max_values(results, edgetypes);
  std::cout << "min:" << min << std::endl;
  std::cout << "max:" << max << std::endl;

  Model::Networkproblem::insert_second_json_in_topology_json(
      topology, results, "results");

  double divideby = 1.0;
  if (argc == 4) {
    divideby = 10;
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

  double max_x = std::numeric_limits<double>::lowest();
  double min_x = std::numeric_limits<double>::max();
  double max_y = std::numeric_limits<double>::lowest();
  double min_y = std::numeric_limits<double>::max();
  for (auto &node : nodes) {

    auto label_id = node.id;
    label_id.erase(0, 5);

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

    outstream << "\\node[draw=" << node.color << "](" << node.id << ") at("
              << out_x << ", " << out_y << "){" << label_id << "};\n";
  }

  double legend_x = max_x + 0.1 * (max_x - min_x);

  for (int i = 0; i != 101; ++i) {

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
      // if (not done) {
      //   std::cout << topology["connections"][type].back().dump(1, '\t');
      //   done = true;
      // }
      std::string from = edge["from"];
      std::string to = edge["to"];
      double value = min;
      if (not edge.contains("results")) {
        throw std::runtime_error("no results!!");
      }
      for (auto const &step : edge["results"]["data"]) {
        for (auto const &[key, quantity] : step.items()) {
          if (key != "pressure") {
            continue;
          }
          for (auto const &xpoint : quantity) {
            if (xpoint["value"].get<double>() > value) {
              value = xpoint["value"].get<double>();
            }
          }
        }
      }
      std::cout << "value: " << value << std::endl;
      auto color = rgb_color(min, max, value);
      outstream << "\\draw[line width=15pt,color={rgb:red," << color.r
                << ";green," << color.g << ";blue," << color.b << "}](" << from
                << ")--(" << to << ");\n";
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

  double min = std::numeric_limits<double>::max();
  double max = std::numeric_limits<double>::lowest();

  for (auto const &type : types) {
    if (not results["connections"].contains(type)) {
      continue;
    }
    for (auto const &component : results["connections"][type]) {
      for (auto &step : component["data"]) {
        for (auto const &[key, quantity] : step.items()) {
          if (key != "pressure") {
            continue;
          }
          double pipe_wide_max = min;
          for (auto const &xpoint : quantity) {
            if (xpoint["value"] > pipe_wide_max) {
              pipe_wide_max = xpoint["value"];
            }
          }
          if (pipe_wide_max < min) {
            min = pipe_wide_max;
          }
          if (pipe_wide_max > max) {
            max = pipe_wide_max;
          }
        }
      }
    }
  }
  return {min, max};
}
