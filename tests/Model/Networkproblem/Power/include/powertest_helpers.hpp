#pragma once
#include <Eigen/Dense>
#include <nlohmann/json.hpp>

nlohmann::json powernode_json(
    std::string id, double G, double B, Eigen::Vector2d bd0,
    Eigen::Vector2d bd1);
nlohmann::json transmissionline_json(
    std::string id, double G, double B, nlohmann::json const &startnode,
    nlohmann::json const &endnode);
