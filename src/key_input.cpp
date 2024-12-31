#include "key_input.h"
#include <SDL_events.h>
#include <string>

bool is_chip8_key(std::string &key_name) {
    if (key_name == "1" || key_name == "2" || key_name == "3" ||
        key_name == "4" || key_name == "5" || key_name == "6" ||
        key_name == "7" || key_name == "8" || key_name == "9" ||
        key_name == "0" || key_name == "A" || key_name == "B" ||
        key_name == "C" || key_name == "D" || key_name == "E" ||
        key_name == "F") {
        return true;
    }

    return false;
}

std::string handle_key_input(SDL_Event *event) {
    if (event->type == SDL_KEYDOWN) {
        std::string key_name = SDL_GetKeyName(event->key.keysym.sym);
        printf("KEY_UP: %s\n", key_name.c_str());

        if (is_chip8_key(key_name)) {
            return key_name;
        }
        return "";

    } else if (event->type == SDL_KEYUP) {
        printf("KEY_DOWN %s\n", SDL_GetKeyName(event->key.keysym.sym));
    }

    return "";
}