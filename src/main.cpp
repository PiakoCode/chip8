#include "chip8.h"
#include "utils.h"
#include <cstdlib>
#include <iostream>
#include <SDL2/SDL.h>


const std::string ROM_PATH = "/home/piako/Codes/base/chip8/rom/test_opcode.ch8";

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout <<"Usage: chip8 [path/to/rom/]";
        exit(1);
    }


    auto buffer = read_binary_file(std::string(argv[1]));
    
    Chip8 chip8;
    chip8.load_rom(buffer.data(), buffer.size());

    std::cout <<"Chip8 start working..." << std::endl;

    chip8.run();


    return 0;
}