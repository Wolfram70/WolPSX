#include <iostream>
#include <sstream>

#include "../include/CPU.hpp"
#include "../include/Bus.hpp"

CPU::CPU()
{
    gpreg[0] = 0; // $zero register
    pc = 0xbfc00000; // Program counter

    lookup_op[0b001111] = &CPU::LUI;
    lookup_op[0b001101] = &CPU::ORI;
    lookup_op[0b101011] = &CPU::SW;
    lookup_op[0b000000] = &CPU::SPECIAL;
    lookup_op[0b001001] = &CPU::ADDIU;
    lookup_op[0b000010] = &CPU::J;

    lookup_special[0b000000] = &CPU::SLL;
    lookup_special[0b100101] = &CPU::OR;
}

void CPU::load_next_ins()
{
    ir = ir_next;
    ir_next = read32(pc);
    ins = Instruction(ir);
    pc += 4;
}

uint32_t CPU::read32(uint32_t addr)
{
    return bus->read32_cpu(addr);
}

void CPU::write32(uint32_t addr, uint32_t data)
{
    bus->write32_cpu(addr, data);
}

void CPU::decode_and_execute()
{
    if(lookup_op.find(ins.opcode()) != lookup_op.end())
    {
        (this->*lookup_op[ins.opcode()])();
        return;
    }
    //throw unhandled instruction error
    std::stringstream ss;
    ss << "Unhandled instruction: " << std::hex << ir;
    throw std::runtime_error(ss.str());
}

void CPU::clock()
{
    load_next_ins();
    decode_and_execute();
    gpreg[0] = 0; // $zero register
}

//INSTRUCTIONS

void CPU::SPECIAL()
{
    if(lookup_special.find(ins.funct()) != lookup_special.end())
    {
        (this->*lookup_special[ins.funct()])();
        return;
    }
    //throw unhandled instruction error
    std::stringstream ss;
    ss << "Unhandled instruction (SPECIAL): " << std::hex << ir;
    throw std::runtime_error(ss.str());
}

void CPU::LUI() //Load Upper Immediate
{
    gpreg[ins.rt()] = ins.imm() << 16;
}

void CPU::ORI() //Bitwise OR Immediate
{
    gpreg[ins.rt()] = gpreg[ins.rs()] | ins.imm();
}

void CPU::SW() //Store Word
{
    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
    {
        offset |= 0xffff0000;
    }
    write32(gpreg[ins.rs()] + offset, gpreg[ins.rt()]);
}

void CPU::SLL() //Shift Left Logical
{
    gpreg[ins.rd()] = gpreg[ins.rt()] << ins.shamt();
}

void CPU::ADDIU() //Add Immediate Unsigned
{
    gpreg[ins.rt()] = gpreg[ins.rs()] + ins.imm();
}

void CPU::J() //Unconditional Jump
{
    pc = (pc & 0xf0000000) | (ins.addr() << 2);
}

void CPU::OR() //Bitwise OR
{
    gpreg[ins.rd()] = gpreg[ins.rs()] | gpreg[ins.rt()];
}