#include "display.h"
#include <SDL_render.h>
#include <cstdint>

Display::Display() { init(); }

Display::~Display() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::init() {
    // 设置环境变量使用wayland
    SDL_setenv("SDL_VIDEODRIVER", "wayland", 1);

    // 初始化SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("初始化失败: %s", SDL_GetError());
        return;
    }

    // 创建窗口
    window = SDL_CreateWindow("CHIP-8",             // 窗口标题
                              SDL_WINDOWPOS_CENTERED, // X位置
                              SDL_WINDOWPOS_CENTERED, // Y位置
                              this->scale * 64,
                              this->scale * 32, // 宽度、高度
                              SDL_WINDOW_SHOWN  // 窗口标志
    );
    // this->pixels[0][0] = 1;
    // 创建渲染器
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void Display::draw() {
    // 绘制像素
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 32; y++) {
            if (pixels[x][y]) {
                SDL_Rect rect = {x * scale, y * scale, scale, scale};
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                // 绘制白色像素
                SDL_RenderFillRect(renderer, &rect);
            } else {
                SDL_Rect rect = {x * scale, y * scale, scale, scale};
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                // 绘制黑色像素
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}


void Display::set_pixel(int x, int y, bool value) { pixels[x][y] = value; }
bool Display::get_pixel(int x, int y) { return pixels[x][y]; }

void Display::render_present() { SDL_RenderPresent(renderer); }

void Display::clear() {
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 32; y++) {
            set_pixel(x, y, 0);
        }
    }
    SDL_RenderClear(renderer);
}
