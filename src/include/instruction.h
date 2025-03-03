#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>


class Sprite {
public:
private:
    uint8_t sprite_value_;
};



class Instruction {
public:
    explicit Instruction(uint16_t opcode);


    uint16_t get_opcode() const;
    void set_opcode(uint16_t opcode);
    uint16_t get_OP() const;
    
    uint8_t get_X() const;
    uint8_t get_Y() const;
    uint8_t get_N() const;
    uint8_t get_NN() const;
    uint16_t get_NNN() const;

private:
    uint16_t opcode_;

    uint16_t OP_;
    uint8_t X_;
    uint8_t Y_;
    uint8_t N_;
    uint8_t NN_;
    uint16_t NNN_;

};

#endif