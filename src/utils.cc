#include <cstdint>
#include "utils.h"
#include <fstream>
#include <random>
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


uint8_t rand_uint8_t() {
        // 使用随机设备生成种子，以保证每次运行时生成不同的随机数
        std::random_device rd;
    
        // 使用Mersenne Twister引擎
        std::mt19937 gen(rd());
        
        std::uniform_int_distribution<uint8_t> dist(0, std::numeric_limits<uint8_t>::max());
        
        // 生成随机数
        uint8_t random_number = dist(gen);
        return random_number;
}