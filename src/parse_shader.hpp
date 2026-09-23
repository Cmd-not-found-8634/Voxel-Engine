#pragma once

#include <fstream>
#include <string>

char* parse_shader(const std::string& file_path){
    std::ifstream file(file_path, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: "+file_path);
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();

    file.seekg(0, std::ios::beg);

    char* data = new char[size + 1];

    file.read(data, size);

    data[size] = '\0';

    return data;
}
