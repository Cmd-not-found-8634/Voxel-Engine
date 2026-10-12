#pragma once

// JSON is used to store the properties of blocks and user configurations
// It is parsed and rendered at runtime so JSON can be changed freely changing the output.

#include <fstream>

#include <utils/json.hpp>

nlohmann::json parse_json(const std::string& file_path){
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: "+file_path);
    }
    nlohmann::json data;
    file>>data;
    return data;
}