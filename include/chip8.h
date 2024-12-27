#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

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
    uint8_t memory[4096]={0};

    // 16 个 8 位 通用寄存器
    uint8_t V[16]={0};

    // 1 个 16 位 地址寄存器
    uint16_t I{0};

    // pc
    uint16_t pc{0x200};

    // 栈
    uint16_t stack[16]={0};

    // 栈指针
    uint16_t sp{0};

    // 屏幕
    uint8_t screen[64][32] = {0};

    // 键盘
    uint8_t keyboard[16]={0};

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

    void run() {
        for (pc = 0; pc < ops.size();pc++) {
            printf("0x%04X\n", ops[pc]);
            exectue(ops[pc]);
            
        }
    }

    void exectue(Instruction ins) {

        switch (ins.opcode) {
        // 执行
        case 0x0:
            if (ins.ins == 0x00E0) { // 清屏
                for (int i = 0; i < 64; i++) {
                    for (int j = 0; j < 32; j++) {
                        set_screen(i, j, 0);
                    }
                }

            } else if (ins.ins == 0x00EE) {
                // TODO:
                // 返回
            } else {
                // TODO:
                // 在地址 NNN 处执行机器语言子程序
                pc = ins.NNN;
            }
            break;
        case 0x1: {
            // 跳转到地址 NNN
            pc = (ins.NNN - 0x200)/2 - 1;
        } break;
        case 0x2: {
            // TODO:
            // 调用子程序 NNN
        } break;
        case 0x3: {
            // 如果寄存器 VX 的值等于 NN ，则跳过下一条指令
            if (V[ins.X] == ins.NN) {
                pc += 1;
            }
        } break;
        case 0x4: {
            // 如果寄存器 VX 的值不等于 NN ，则跳过下一条指令
            if (V[ins.X] != ins.NN) {
                pc += 1;
            }
        } break;
        case 0x5: {
            // 如果寄存器 VX 的值等于寄存器 VY 的值，则跳过下一条指令
            if (V[ins.X] == V[ins.Y]) {
                pc += 1;
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
                if (V[ins.Y] > (0xFF - V[ins.X])) {
                    V[0xF] = 1;
                } else {
                    V[0xF] = 0;
                }
                V[ins.X] += V[ins.Y];
                break;
            case 0x5:
                // 从 寄存器 VX 中减去寄存器 VY 的值，VF 设置为借位标志
                if (V[ins.Y] > V[ins.X]) {
                    V[0xF] = 1;
                } else {
                    V[0xF] = 0;
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
                V[0xF] = V[ins.Y] >> 7;
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
                pc += 1;
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
            V[ins.X] = (rand() % 256) & ins.NN;
        } break;
        case 0xD: {
            // 在位置 VX , VY 绘制一个精灵，使用存储在 I 地址的 N
            // 字节的精灵数据,如果任何设置的像素被取消设置，则将 VF 设置为 01
            // ，否则设置为 00
            uint8_t x = V[ins.X];
            uint8_t y = V[ins.Y];
            uint8_t addr = I;
            uint8_t n = ins.N;
            // TODO:
        } break;
        case 0xE: {
            if (ins.NN == 0x9E) {

            } else if (ins.NN == 0xA1) {
                // 如果键 VX 被按下，则跳过下一条指令
                if (keyboard[V[ins.X]]) {
                    pc++;
                }
            } else {
                printf("Unknown opcode: 0x%X\n", ins.ins);
            }
        } break;
        case 0xF: {
            if (ins.NN == 0x07) {
                V[ins.X] = delay_timer;
            } else if (ins.NN == 0x0A) {
                // TODO:
            } else if (ins.NN == 0x15) {
                delay_timer = V[ins.X];
            } else if (ins.NN == 0x18) {
                sound_timer = V[ins.X];
            } else if (ins.NN == 0x1E) {
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
