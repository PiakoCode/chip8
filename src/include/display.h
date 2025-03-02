#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL_render.h>
#pragma once

#include <SDL2/SDL.h>

class Display {

public:
    Display();
    ~Display();    
    void init();
    void loop();
    void draw();
    void clear();
    void set_pixel(int x, int y, bool value);
    bool get_pixel(int x, int y);
    void render_present();
    
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool pixels[64][32]{0};  // 显示缓冲区
    int scale{15};            // 像素缩放比例
};




#endif