#include "chip8.h"
#include "display.h"
#include "key_input.h"
#include "utils.h"
#include <SDL_timer.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <thread>
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
        } else if (instruction_.get_opcode() == 0x00EE) {
            // 栈程序返回（出栈）
            this->pc_ = this->stack_[this->sp_];
            this->sp_--;
        }
        break;
    case 0x1:
        // 跳转到地址 NNN
        this->pc_ = instruction_.get_NNN();

        break;
    case 0x2:
        // 调用子程序(入栈)
        sp_++;
        this->stack_[this->sp_] = this->pc_;
        this->pc_ = instruction_.get_NNN();
        break;

    case 0x3:
        // 如果寄存器 VX 等于 NN，则跳过下一个指令
        if (VX_ref == instruction_.get_NN()) {
            this->pc_ += 2;
        }
        break;

    case 0x4:
        // 如果寄存器 VX 不等于 NN，则跳过下一个指令
        if (VX_ref != instruction_.get_NN()) {
            this->pc_ += 2;
        }

        break;

    case 0x5:
        if (instruction_.get_N() == 0) {
            // 如果寄存器 VX 等于 VY，则跳过下一个指令
            if (VX_ref == VY_ref) {
                this->pc_ += 2;
            }
        }
        break;

    case 0x6:
        // 设置寄存器 VX 的值为 NN
        VX_ref = instruction_.get_NN();

        break;

    case 0x7:
        // 寄存器 VX 增加 NN(不设置溢出标志)
        VX_ref += instruction_.get_NN();

        break;

    case 0x8:
        switch (instruction_.get_N()) {
        case 0x0:
            // 将 VX 的值设置为 VY 的值
            VX_ref = VY_ref;
            break;
        case 0x1:
            // 将 VX 的值设置为 VX 和 VY 的按位或
            VX_ref = VX_ref | VY_ref;
            break;
        case 0x2:
            // 将 VX 的值设置为 VX 和 VY 的按位与
            VX_ref = VX_ref & VY_ref;
            break;
        case 0x3:
            // 将 VX 的值设置为 VX 和 VY 的按位异或
            VX_ref = VX_ref ^ VY_ref;
            break;
        case 0x4:
            // 将 VX 的值设置为 VX 和 VY 的和(需要设置溢出标志)

            // 如果会发生溢出
            if (0xFF - VX_ref < VY_ref || 0xFF - VY_ref < VX_ref) {
                this->v_[0xF] = 1;
            } else {
                this->v_[0xF] = 0;
            }
            VX_ref += VY_ref;

            break;
        case 0x5:
            // 将 VX 的值设置为 VX 减 VY(需要设置借位标志)
            if (VX_ref > VY_ref) {
                this->v_[0xF] = 1;
            } else {
                this->v_[0xF] = 0;
            }
            VX_ref = VX_ref - VY_ref;
            break;

        case 0x6:
            // HACK: 多种实现方式(模糊指令)
            // VX_ref = VY_ref
            if ((VX_ref & 0x1) == 0x1) {
                v_[0xF] = 1;
            } else {
                v_[0xF] = 0;
            }
            VX_ref = VX_ref >> 1;
            break;
        case 0x7:
            // 将 VX 的值设置为 VY 减 VX(需要设置借位标志)
            if (VY_ref > VX_ref) {
                this->v_[0xF] = 1;
            } else {
                this->v_[0xF] = 0;
            }
            VX_ref = VY_ref - VX_ref;
            break;
        case 0xE:
            // HACK: 多种实现方式(模糊指令)
            // VX_ref = VY_ref
            if ((VX_ref & 0x80) == 0x80) {
                v_[0xF] = 1;
            } else {
                v_[0xF] = 0;
            }
            VX_ref = VX_ref << 1;
            break;
        }
        break;

    case 0x9:

        break;

    case 0xA:
        // 设置索引寄存器的值为 NNN
        this->index_reg_ = instruction_.get_NNN();
        break;

    case 0xB:
        // HACK: 多种实现方式(模糊指令)
        // 跳转到地址 NNN + V0
        // this->pc_ = instruction_.get_NNN() + this->v_[0];
        // 或者
        this->pc_ = instruction_.get_NNN() + VX_ref;
        break;

    case 0xC: {
        // 生成一个随机数，与 NN 进行按位与，将结果存入 VX
        uint8_t number = rand_uint8_t();
        VX_ref = number & instruction_.get_NN();
    }

    break;

    case 0xD: {
        // 它将从内存位置中绘制一个 N 像素高的精灵到屏幕上，水平 X 坐标在 VX ，Y
        // 坐标在 VY
        // 。精灵中所有“开启”的像素将翻转屏幕上绘制的像素（从左到右，从最高位到最低位）。如果屏幕上的任何像素因此被关闭，则
        // VF 标志寄存器设置为 1。否则，设置为 0。

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
                    // printf("X: %d, Y: %d\n", x, y);
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
            if (y >= 32) {
                // 超出屏幕范围后停止绘制
                break;
            }
        }
    }

    break;

    case 0xE:
        if (instruction_.get_NN() == 0x9E) {
            // 如果按下与 VX 中值对应的键， EX9E 将跳过一条指令（PC 增加 2）
            if (this->key_[VX_ref] == 1) {
                this->pc_ += 2;
                clean_key_state();
            }
        }

        if (instruction_.get_NN() == 0xA1) {
            // 如果没有按下与 VX 中值对应的键， EXA1 将跳过一条指令（PC 增加 2）
            if (this->key_[VX_ref] == 0) {
                this->pc_ += 2;
                clean_key_state();
            }
        }
        break;

    case 0xF:
        switch (instruction_.get_NN()) {
        // Timer
        case 0x07:
            // 将VX设置为延时计数器的当前值
            VX_ref = this->delay_timer_;
            break;

        case 0x15:
            // 将延时计数器设置为VX的值
            this->delay_timer_ = VX_ref;
            break;

        case 0x18:
            // 将声音计数器设置为VX的值
            this->sound_timer_ = VX_ref;
            break;

        case 0x1E:
            // Add VX_value to index
            if (0xFF - this->index_reg_ < VX_ref) {
                this->v_[0xF] = 1;
            } else {
                this->v_[0xF] = 0;
            }
            this->index_reg_ += VX_ref;

            break;

        case 0x0A:
            // Get key
            {
                bool flag = false;
                for (uint8_t i = 0; i < 16; i++) {
                    if (key_[i] != 0) { // 如果有按键按下
                        flag = true;
                        VX_ref = i; // 将按键值存入VX
                        break;
                    }
                }
                if (!flag) {
                    this->pc_ -= 2; // 如果没有按键按下，pc-2 进入循环
                } else {
                    clean_key_state();
                }
            }

            break;

        case 0x29:
            // Font character
            this->index_reg_ = 0x050 + VX_ref * 5;
            break;

        case 0x33:
            // Binary-coded decimal conversion
            {
                uint8_t value = VX_ref;
                for (int i = 2; i >= 0; i--) {
                    *(this->memory_ + this->index_reg_ + i) = value % 10;
                    value /= 10;
                }
            }
            break;
        // Store and load memory
        case 0x55:
            for (int i = 0; i <= instruction_.get_X(); i++) {
                *(this->memory_ + this->index_reg_ + i) = this->v_[i];
            }
            break;
        case 0x65:
            for (int i = 0; i <= instruction_.get_X(); i++) {
                this->v_[i] = *(this->memory_ + this->index_reg_ + i);
            }

            break;
        }
        break;

    default:
        break;
    }
}

void Chip8::clean_key_state() {
    // clean key state
    memset(this->key_, 0, sizeof(this->key_));
}

void Chip8::run() {

    std::thread decode_thread = std::thread([&]{
        while (true) {


        /* 执行指令*/
        instruction_ = Instruction(fetch());
        this->pc_ += 2; // pc increase
        decode_execute();
        /********/

        // display_.draw();
        // display_.render_present();

        // 计数器
        this->sound_timer_--;
        this->delay_timer_--;
        
        // 线程休眠
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); 

        }
    });
    draw_thread.detach();


    // 主循环
    SDL_bool quit = SDL_FALSE;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = SDL_TRUE;
            }
        }
        auto key = get_keyborad_state();
        auto key_code = keyboard_map(key);
        if (key_code != 0xFF) {
            this->key_[key_code] = 1;
        }

        display_.draw();
        display_.render_present();
        // SDL_Delay(1);
        

    }
}