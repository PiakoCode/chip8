#include "include/key_input.h"
#include <SDL_events.h>
#include <cstdint>
#include <string>

std::string get_keyborad_state() {
    const auto state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_1]) {
        return "1";
    }
    if (state[SDL_SCANCODE_2]) {
        return "2";
    }
    if (state[SDL_SCANCODE_3]) {
        return "3";
    }
    if (state[SDL_SCANCODE_4]) {
        return "4";
    }
    if (state[SDL_SCANCODE_Q]) {
        return "Q";
    }
    if (state[SDL_SCANCODE_W]) {
        return "W";
    }
    if (state[SDL_SCANCODE_E]) {
        return "E";
    }
    if (state[SDL_SCANCODE_R]) {
        return "R";
    }
    if (state[SDL_SCANCODE_A]) {
        return "A";
    }
    if (state[SDL_SCANCODE_S]) {
        return "S";
    }
    if (state[SDL_SCANCODE_D]) {
        return "D";
    }
    if (state[SDL_SCANCODE_F]) {
        return "F";
    }
    if (state[SDL_SCANCODE_Z]) {
        return "Z";
    }
    if (state[SDL_SCANCODE_X]) {
        return "X";
    }
    if (state[SDL_SCANCODE_C]) {
        return "C";
    }
    if (state[SDL_SCANCODE_V]) {
        return "V";
    }
    return "";
}


uint8_t keyboard_map(std::string key) {
    if (key == "1") {
        return 0x1;
    }
    if (key == "2") {
        return 0x2;
    }
    if (key == "3") {
        return 0x3;
    }
    if (key == "4") {
        return 0xC;
    }
    if (key == "Q") {
        return 0x4;
    }
    if (key == "W") {
        return 0x5;
    }
    if (key == "E") {
        return 0x6;
    }
    if (key == "R") {
        return 0xD;
    }
    if (key == "A") {
        return 0x7;
    }
    if (key == "S") {
        return 0x8;
    }
    if (key == "D") {
        return 0x9;
    }
    if (key == "F") {
        return 0xE;
    }
    if (key == "Z") {
        return 0xA;
    }
    if (key == "X") {
        return 0x0;
    }
    if (key == "C") {
        return 0xB;
    }
    if (key == "V") {
        return 0xF;
    }
    return 0xFF;
}


