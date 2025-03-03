#include "instruction.h"
#include <cstdint>

Instruction::Instruction(uint16_t opcode) {
    this->opcode_ = opcode;
    this->OP_ = (opcode & 0xF000) >> 12;
    this->X_ = (opcode & 0x0F00) >> 8;
    this->Y_ = (opcode & 0x00F0) >> 4;
    this->N_ = opcode & 0x000F;
    this->NN_ = opcode & 0x00FF;
    this->NNN_ = opcode & 0x0FFF;
}


uint16_t Instruction::get_opcode() const {
    return this->opcode_;
}

void Instruction::set_opcode(uint16_t opcode) {
    this->opcode_ = opcode;
}

uint16_t Instruction::get_OP() const {
    return this->OP_;
}

uint8_t Instruction::get_X() const {
    return this->X_;
}

uint8_t Instruction::get_Y() const {
    return this->Y_;
}

uint8_t Instruction::get_N() const {
    return this->N_;
}

uint8_t Instruction::get_NN() const {
    return this->NN_;
}

uint16_t Instruction::get_NNN() const {
    return this->NNN_;
}
