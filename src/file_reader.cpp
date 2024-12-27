#include "file_reader.h"
#include <cstdint>
#include <fstream>
#include <vector>

std::vector<uint8_t> read_binary_file(const std::string &file_path) {
    auto file = std::ifstream(file_path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file");
    }
    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(static_cast<size_t>(file_size));
    file.read(reinterpret_cast<char *>(buffer.data()), file_size);
    file.close();
    return buffer;
}
