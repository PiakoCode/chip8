#include "chip8.h"
#include "display.h"
#include <cstdint>
#include <cstdio>
#include <unistd.h>

Chip8::Chip8() {
    // font memory init
    // 0x050 – 0x09F
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 5; j++) {
            this->memory_[0x50 + i * 5 + j] = FONT_SPRITES[i][j];
        }
    }
}

void Chip8::load_rom(const uint8_t *buffer, size_t size) {
    for (int i = 0; i < size; i++) {
        this->memory_[0x200 + i] = buffer[i];
    }
}

uint16_t Chip8::fetch() {

    printf("=== fetch ===\n");
    printf("pc: 0x%X\n", this->pc_);
    printf("memory: 0x%X\n", *this->memory_ + this->pc_);
    uint16_t ins =
        *(this->memory_ + this->pc_) << 8 | (*(this->memory_ + this->pc_ + 1));
    printf("ins: 0x%X\n", ins);
    this->pc_ += 2;
    return ins;
}

void Chip8::decode_execute() {
    // TODO
    auto &VX_ref = this->v_[this->instruction_.get_X()];
    auto &VY_ref = this->v_[this->instruction_.get_Y()];

    switch (instruction_.get_OP()) {
    case 0x0:
        if (instruction_.get_opcode() == 0x00E0) {
            this->display_.clear();
        }
        break;
    case 0x1:
        // 跳转到地址 NNN
        this->pc_ = instruction_.get_NNN();

        break;
    case 0x2:
        break;

    case 0x3:

        break;

    case 0x4:

        break;

    case 0x5:

        break;

    case 0x6:
        // 设置寄存器 VX 的值为 NN
        VX_ref = instruction_.get_NN();

        break;

    case 0x7:
        // 寄存器 VX 增加 NN
        VX_ref += instruction_.get_NN();

        break;

    case 0x8:

        break;

    case 0x9:

        break;

    case 0xA:
        // 设置索引寄存器的值为 NNN
        this->index_reg_ = instruction_.get_NNN();
        break;

    case 0xB:

        break;

    case 0xC:

        break;

    case 0xD:
        // 它将从内存位置中绘制一个 N 像素高的精灵到屏幕上，水平 X 坐标在 VX ，Y
        // 坐标在 VY
        // 。精灵中所有“开启”的像素将翻转屏幕上绘制的像素（从左到右，从最高位到最低位）。如果屏幕上的任何像素因此被关闭，则
        // VF 标志寄存器设置为 1。否则，设置为 0。
        {
            uint8_t x = VX_ref % 64;
            uint8_t y = VY_ref % 32;
            this->v_[0xF] = 0;
            for (int i = 0; i < instruction_.get_N(); i++) {
                // 获取精灵数据的第n个字节
                uint8_t sprite_byte = *(this->memory_ + this->index_reg_ + i);
                // printf("sprite_byte: 0x%X\n", sprite_byte);
                // 对此字节的每个位进行判断
                for (int j = 0; j < 8; j++) {

                    uint8_t sprite_bit = (sprite_byte >> (7 - j)) & 0x1;
                    uint8_t pixel_bit = this->display_.get_pixel(x, y);
                    // 如果此位为1，且屏幕上的像素也为1，则设置VF为1
                    if (sprite_bit == 1 && pixel_bit == 1) {
                        this->display_.set_pixel(x, y, 0);
                        this->v_[0xF] = 1;
                    } else if (sprite_bit == 1 && pixel_bit == 0) {
                        //printf("X: %d, Y: %d\n", x, y);
                        this->display_.set_pixel(x, y, 1);
                    }

                    x++;
                    if (x >= 64) {
                        // 超出屏幕范围后停止绘制
                        break;
                    }
                }
                // 绘制下一行, x 重置为 VX
                x = VX_ref % 64;

                y++;
                if (y>=32) {
                    // 超出屏幕范围后停止绘制
                    break;
                }
            }
        }

        break;

    case 0xE:

        break;

    case 0xF:

        break;

    default:
        break;
    }
}

void Chip8::run() {

    // 主循环
    SDL_bool quit = SDL_FALSE;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = SDL_TRUE;
            }
        }

        /* 执行指令*/
        instruction_ = Instruction(fetch());
        decode_execute();
        /********/

        // 根据pixles数组绘制屏幕
        display_.draw();

        display_.render_present();
    }
}