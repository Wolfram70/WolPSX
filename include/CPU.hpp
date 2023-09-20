#ifndef CPU_HPP
#define CPU_HPP

#include <stdint.h>
#include <map>

class Bus;

struct Instruction
{
    uint32_t ins;

    Instruction() {}
    Instruction(uint32_t ins) : ins(ins) {}
    uint32_t opcode() { return ins >> 26; } // Bits 31-26
    uint32_t rs() { return (ins >> 21) & 0x1f; } // Bits 25-21
    uint32_t rt() { return (ins >> 16) & 0x1f; } // Bits 20-16
    uint32_t rd() { return (ins >> 11) & 0x1f; } // Bits 15-11
    uint32_t shamt() { return (ins >> 6) & 0x1f; } // Bits 10-6
    uint32_t funct() { return ins & 0x3f; } // Bits 5-0
    uint32_t imm() { return ins & 0xffff; } // Bits 15-0
    uint32_t addr() { return ins & 0x3ffffff; } // Bits 25-0
};

class CPU
{
public:
    CPU();
    void connectBus(Bus* bus) { this->bus = bus; }
    void clock();

private:
    void load_next_ins();
    void decode_and_execute();

    uint32_t read32(uint32_t addr);
    void write32(uint32_t addr, uint32_t data);

private:
    Bus* bus;

    uint32_t pc = 0; // Program counter
    uint32_t ir = 0;
    uint32_t ir_next = 0;
    Instruction ins;
    uint32_t gpreg[32] = {0xdeaddeed}; // General purpose registers
    uint32_t hi = 0; // Mult/Div registers (higher 32 bits)
    uint32_t lo = 0; // Mult/Div registers (lower 32 bits)

private:
    std::map<uint8_t, void (CPU::*)()> lookup_op; // Lookup table for instructions
    std::map<uint8_t, void (CPU::*)()> lookup_special; // Lookup table for special instructions

    void LUI();
    void ORI();
    void SW();
    void ADDIU();
    void J();

    void SPECIAL();
    void SLL();
    void OR();
};

#endif