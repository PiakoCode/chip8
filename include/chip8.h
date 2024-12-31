#pragma once

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>
#include <vector>

static uint8_t fontROM[] = {
    // 4x5 font sprites (0-F)
    0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20, 0x70, 0xF0, 0x10,
    0xF0, 0x80, 0xF0, 0xF0, 0x10, 0xF0, 0x10, 0xF0, 0xA0, 0xA0, 0xF0, 0x20,
    0x20, 0xF0, 0x80, 0xF0, 0x10, 0xF0, 0xF0, 0x80, 0xF0, 0x90, 0xF0, 0xF0,
    0x10, 0x20, 0x40, 0x40, 0xF0, 0x90, 0xF0, 0x90, 0xF0, 0xF0, 0x90, 0xF0,
    0x10, 0xF0, 0xF0, 0x90, 0xF0, 0x90, 0x90, 0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0xF0, 0x80,
    0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80,
};

class Instruction {
public:
    uint16_t ins;
    uint8_t  opcode;
    uint8_t  X;
    uint8_t  Y;
    uint8_t  N;
    uint8_t  NN;
    uint16_t NNN;

    Instruction(uint16_t ins) {
        this->ins = ins;
        opcode = getOpcode();
        X = getX();
        Y = getY();
        N = getN();
        NN = getNN();
        NNN = getNNN();
    }

    uint8_t getOpcode() { return (ins & 0xF000) >> 12; }

    uint8_t getX() { return (ins & 0x0F00) >> 8; }

    uint8_t getY() { return (ins & 0x00F0) >> 4; }

    uint8_t getN() { return ins & 0x000F; }

    uint8_t getNN() { return ins & 0x00FF; }

    uint16_t getNNN() { return ins & 0x0FFF; }
};

class VM {
public:
    /**
     * @brief
     * 内存 大小为 4KB，即 4096 字节
     * 其中 0x000 - 0x1FF 为系统保留区域
     * 0x200 - 0xFFF 为程序区域
     */
    uint8_t memory[4096] = {0};

    // 16 个 8 位 通用寄存器
    uint8_t V[16] = {0};

    // 1 个 16 位 地址寄存器
    uint16_t I{0};

    // pc
    uint16_t pc{0x200};

    // 栈
    uint16_t stack[16] = {0};

    // 栈指针
    uint16_t sp{0};

    // 屏幕
    uint8_t screen[64][32] = {0};

    // 键盘
    uint8_t keyboard[16] = {0};

    // 定时器
    uint8_t delay_timer;
    uint8_t sound_timer;

    std::vector<uint16_t> ops;

    void set_screen(int x, int y, uint8_t value) {
        if (x >= 64 || y >= 32) {
            return;
        }
        screen[x][y] = value;
    }

    std::map<std::string, uint8_t> key_map = {{"0", 0},
                                              {"1", 1},
                                              {"2", 2},
                                              {"3", 3},
                                              {"4", 4},
                                              {"5", 5},
                                              {"6", 6},
                                              {"7", 7},
                                              {"8", 8},
                                              {"9", 9},
                                              {"A", 10},
                                              {"B", 11},
                                              {"C", 12},
                                              {"D", 13},
                                              {"E", 14},
                                              {"F", 15}};

    void mem2screen() {
        int begin_pos = 0xf00;
        for (int i = 0; i < 64 * 32; i++) {
            int x = i % 64;
            int y = i / 64;
            set_screen(x, y, memory[begin_pos + i]);
        }
    }

    void screen2mem() {
        int begin_pos = 0xf00;
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 32; j++) {
                memory[begin_pos + i * 32 + j] = screen[i][j];
            }
        }
    }

    void clear_screen() {
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 32; j++) {
                set_screen(i, j, 0);
            }
        }
        screen2mem();
    }

    void run() {
        for (pc = 0x200; pc < 0x200 + ops.size() * 2; pc += 2) {
            // pc 转换为 ops数组的index
            uint16_t index = (pc - 0x200) / 2;
            printf("0x%04X\n", ops[index]);
            exectue(ops[index]);
        }
    }

    void exectue(Instruction ins) {

        switch (ins.opcode) {
        // 执行
        case 0x0:
            if (ins.ins == 0x00E0) {
                // 清屏
                clear_screen();

            } else if (ins.ins == 0x00EE) {
                // 返回
                assert(sp > 0);
                pc = stack[--sp];
            } else {
                // 在地址 NNN 处执行机器语言子程序
                pc = ins.NNN;
            }
            break;
        case 0x1: {
            // 跳转到地址 NNN
            pc = ins.NNN;
        } break;
        case 0x2: {
            // 调用子程序 NNN
            stack[sp++] = pc; // 保存返回地址
            pc = ins.NNN;
        } break;
        case 0x3: {
            // 如果寄存器 VX 的值等于 NN ，则跳过下一条指令
            if (V[ins.X] == ins.NN) {
                pc += 2;
            }
        } break;
        case 0x4: {
            // 如果寄存器 VX 的值不等于 NN ，则跳过下一条指令
            if (V[ins.X] != ins.NN) {
                pc += 2;
            }
        } break;
        case 0x5: {
            // 如果寄存器 VX 的值等于寄存器 VY 的值，则跳过下一条指令
            if (V[ins.X] == V[ins.Y]) {
                pc += 2;
            }
        } break;
        case 0x6: {
            // 将数字 NN 存储在寄存器 VX 中
            V[ins.X] = ins.NN;
        } break;
        case 0x7: {
            // 将值 NN 加到寄存器 VX
            V[ins.X] += ins.NN;
        } break;
        case 0x8: {
            switch (ins.N) {
            case 0x0:
                V[ins.X] = V[ins.Y];
                break;
            case 0x1:
                V[ins.X] |= V[ins.Y];
                break;
            case 0x2:
                V[ins.X] &= V[ins.Y];
                break;
            case 0x3:
                V[ins.X] ^= V[ins.Y];
                break;
            case 0x4:
                // 将寄存器 VY 的值加到寄存器 VX 上，VF 设置为进位标志
                if (V[ins.Y] > (0xFF - V[ins.X])) { // 即 VX + VY > 255
                    V[0xF] = 1;
                } else {
                    V[0xF] = 0;
                }
                V[ins.X] += V[ins.Y];
                break;
            case 0x5:
                // 从 寄存器 VX 中减去寄存器 VY 的值，VF 设置为借位标志
                if (V[ins.Y] > V[ins.X]) {
                    V[0xF] = 0;
                } else {
                    V[0xF] = 1;
                }
                V[ins.X] -= V[ins.Y];
                break;
            case 0x6:
                V[0xF] = V[ins.Y] & 0x1;
                V[ins.X] = V[ins.Y] >> 1;
                break;
            case 0x7:
                if (V[ins.Y] < V[ins.X]) {
                    V[0xF] = 0;
                } else {
                    V[0xF] = 1;
                }
                V[ins.X] = V[ins.Y] - V[ins.X];
                break;
            case 0xE:
                V[0xF] = V[ins.X] >> 7 & 0x1;
                V[ins.X] = V[ins.Y] << 1;
                break;
            default:
                printf("Unknown opcode: 0x%X\n", ins.ins);
                break;
            }
        } break;
        case 0x9: {
            // 如果寄存器 VX 的值不等于寄存器 VY 的值，则跳过下一条指令
            if (V[ins.X] != V[ins.Y]) {
                pc += 2;
            }
        } break;
        case 0xA: {
            // 将内存地址 NNN 存储到寄存器 I 中
            I = ins.NNN;
        } break;
        case 0xB: {
            // 跳转到地址 NNN + V0
            pc = ins.NNN + V[0];
        } break;
        case 0xC: {
            // 将 VX 设置为一个随机数，掩码为 NN
            srand(time(0));
            V[ins.X] = (rand() % 256) & ins.NN;
        } break;
        case 0xD: {
            // 在位置 VX , VY 绘制一个精灵，使用存储在 I 地址的 N
            // 字节的精灵数据,如果任何设置的像素被取消设置，则将 VF 设置为 01
            // ，否则设置为 00
            uint8_t x = V[ins.X] % 64;  // 处理x边界，使用模运算实现wrap-around
            uint8_t y = V[ins.Y] % 32;  // 处理y边界，使用模运算实现wrap-around
            uint8_t n = ins.N;          // 获取精灵高度
            V[0xF] = 0;                 // 初始化VF为0，表示没有碰撞
            
            // 逐行绘制精灵
            for (uint8_t row = 0; row < n; row++) {
                // 从内存中读取精灵数据
                uint8_t spriteByte = memory[I + row];
                
                // 逐列绘制精灵
                for (uint8_t col = 0; col < 8; col++) {
                    // 检查当前位是否为1（需要绘制）
                    if ((spriteByte & (0x80 >> col)) != 0) {
                        // 计算像素位置，处理边界
                        uint8_t pixelX = (x + col) % 64;
                        uint8_t pixelY = (y + row) % 32;
                        
                        // 检测像素碰撞
                        if (screen[pixelX][pixelY] == 1) {
                            V[0xF] = 1;  // 设置碰撞标志
                        }
                        
                        // XOR像素（实现精灵绘制）
                        screen[pixelX][pixelY] ^= 1;
                    }
                }
            }
        } break;
        case 0xE: {// 处理键盘输入
            if (ins.NN == 0x9E) {
                // 如果键 VX 被按下，则跳过下一条指令
                if (keyboard[V[ins.X]] == 1) {
                    pc += 2;
                }
            } else if (ins.NN == 0xA1) {
                // 如果键 VX 未被按下，则跳过下一条指令
                if (keyboard[V[ins.X]] == 0) {
                    pc += 2;
                }
            } else {
                printf("Unknown opcode: 0x%X\n", ins.ins);
            }
        } break;
        case 0xF: { // 处理定时器、声音等
            if (ins.NN == 0x07) {
                V[ins.X] = delay_timer;
            } else if (ins.NN == 0x0A) {
                // 等待按键输入，并将按键值存入 VX
                bool key_pressed = false;
                while (!key_pressed) {
                    for (int i = 0; i < 16; i++) {
                        if (keyboard[i] == 1) {
                            V[ins.X] = i;
                            key_pressed = true;
                            break;
                        }
                    }
                }
            } else if (ins.NN == 0x15) {
                delay_timer = V[ins.X];
            } else if (ins.NN == 0x18) {
                sound_timer = V[ins.X];
            } else if (ins.NN == 0x1E) {
                // 将寄存器 VX 中的值加到寄存器 I 中
                I += V[ins.X];
            } else if (ins.NN == 0x29) {
                // TODO:
            } else if (ins.NN == 0x33) {
                memory[I] = V[ins.X] / 100;
                memory[I + 1] = (V[ins.X] / 10) % 10;
                memory[I + 2] = V[ins.X] % 10;
            } else if (ins.NN == 0x55) {
                // TODO:
            } else if (ins.NN == 0x65) {
                // 将从地址 I 开始的内存中的值填充到寄存器 V0 到 VX 中
                // TODO:
            } else {
                printf("Unknown opcode: 0x%X\n", ins.ins);
            }
        } break;
        default:
            printf("Unknown opcode: 0x%X\n", ins.ins);
            break;
        }
    }
};
