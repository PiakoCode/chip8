#pragma once

#include <cstdint>
#include <string>
#include <vector>

std::vector<uint8_t> read_binary_file(const std::string &file_path);

uint8_t rand_uint8_t();
