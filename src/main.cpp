#include "chip8.h"
#include "display.h"
#include "file_reader.h"
#include <iostream>
#include <SDL2/SDL.h>


int main() {
    auto buffer = read_binary_file("/home/piako/Codes/base/chip8/rom/IBM_Logo.ch8");
    


    Chip8 chip8;
    chip8.load_rom(buffer.data(), buffer.size());

    std::cout <<"Chip8 start working..." << std::endl;

    chip8.run();


    return 0;
}