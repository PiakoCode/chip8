#include "chip8_display.h"
#include "SDL2/SDL.h"
#include <SDL2/SDL_video.h>
#include <SDL_render.h>
#include <cstdlib>

const int block_size = 15;

SDL_Window *screen_init_window() {
    setenv("SDL_VIDEODRIVER", "wayland,x11", 1);

    // 初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow("CHIP-8",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          64 * block_size,
                                          32 * block_size,
                                          SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    return window;
}

SDL_Renderer *screen_init_renderer(SDL_Window *window) {

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n",
               SDL_GetError());
        exit(1);
    }

    return renderer;
}

void screen_dispaly(VM &vm, SDL_Renderer *renderer) {
    // 清除屏幕
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            SDL_Rect rect = {
                i * block_size, j * block_size, block_size, block_size};
            if (vm.screen[i][j] != 0) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            }

            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_RenderPresent(renderer);
}

void screen_quit(SDL_Renderer *renderer, SDL_Window *window) {
    // 清理资源
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}