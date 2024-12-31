#pragma once

#include <SDL2/SDL.h>
#include <string>

std::string handle_key_input(SDL_Event *event);
bool is_chip8_key(std::string &key_name);
