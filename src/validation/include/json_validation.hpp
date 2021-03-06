#pragma once
#include <nlohmann/json.hpp>

bool validate_problem_data(json problem_data);

bool validate_topology(json topology);