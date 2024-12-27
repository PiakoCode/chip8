#include "chip8.h"
#include <SDL_events.h>
#include <iostream>
#include "chip8_display.h"
#include <SDL2/SDL.h>


int main() {
    VM vm;

    auto window = screen_init_window();
    auto renderer =  screen_init_renderer(window);
    int  quit = 0;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
        screen_dispaly(vm, renderer);
        // vm.set_screen(0, 0, 1);
        // vm.set_screen(0, 31, 1);
        // vm.set_screen(63, 0, 1);
        // vm.set_screen(63, 31, 1);

    }
    screen_quit(renderer,window);
    return 0;
}