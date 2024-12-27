#pragma once

#include "chip8.h"
#include <SDL2/SDL.h>
#include <SDL_render.h>

SDL_Renderer *screen_init_renderer(SDL_Window *window);
SDL_Window   *screen_init_window();
void          screen_dispaly(VM &vm, SDL_Renderer *renderer);
void          screen_quit(SDL_Renderer *renderer, SDL_Window *window);
