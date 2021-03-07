#pragma once
#include <nlohmann/json.hpp>


bool validate_json(nlohmann::json data, std::string location);
