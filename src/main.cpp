#include "chip8.h"
#include <SDL_events.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include "chip8_display.h"
#include "file_reader.h"
#include <SDL2/SDL.h>
#include <thread>
#include <vector>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Usage: %s <rom>\n", argv[0]);
        return 1;
    }
    auto file_path = std::string(argv[1]);

    auto                  buffer = read_binary_file(file_path);
    std::vector<uint16_t> ops;
    for (int i = 0; i < buffer.size(); i += 2) {

        uint16_t ins = (buffer[i] << 8) + buffer[i + 1];
        
        ops.push_back(ins);
    }

    VM vm;
    vm.ops = ops;

    vm.run();
    // TODO:

    auto      window = screen_init_window();
    auto      renderer = screen_init_renderer(window);
    int       quit = 0;
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
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    screen_quit(renderer, window);
    return 0;
}