#include <iostream>
#include <sstream>

#include "../include/CPU.hpp"
#include "../include/Bus.hpp"

CPU::CPU()
{
    gpreg_in[0] = 0; // $zero register
    gpreg_out[0] = 0; //$zero register
    pc = 0xbfc00000; // Program counter

    cop0_bda = 0x00000000;
    cop0_bpcm = 0x00000000;
    cop0_bpc = 0x00000000;
    cop0_dcic = 0x00000000;
    cop0_bdam = 0x00000000;
    cop0_status = 0x00000000;
    cop0_cause = 0x00000000;

    pending_load = LoadDelay(0, 0);

    lookup_op[0b000000] = &CPU::SPECIAL;
    lookup_op[0b010000] = &CPU::COP0;
    lookup_op[0b010001] = &CPU::COP1;
    lookup_op[0b010010] = &CPU::COP2;
    lookup_op[0b010011] = &CPU::COP3;

    lookup_op[0b001111] = &CPU::LUI;
    lookup_op[0b001101] = &CPU::ORI;
    lookup_op[0b101011] = &CPU::SW;
    lookup_op[0b001001] = &CPU::ADDIU;
    lookup_op[0b000010] = &CPU::J;
    lookup_op[0b000101] = &CPU::BNE;
    lookup_op[0b001000] = &CPU::ADDI;
    lookup_op[0b100011] = &CPU::LW;

    lookup_special[0b000000] = &CPU::SLL;
    lookup_special[0b100101] = &CPU::OR;

    lookup_cop0[0b00100] = &CPU::MTC0;
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
    //All the reg data req by the instruction is "read already" : Implementation with 2 sets of registers
    load_next_ins();
    set_reg(pending_load.reg, pending_load.data);
    pending_load = LoadDelay(0, 0);
    decode_and_execute();
    gpreg_in[0] = 0;
    gpreg_out[0] = 0; // $zero register
    copy_regs();
    //All the reg data to write is written : Implementation with 2 sets of registers
}

void CPU::branch(uint32_t offset)
{
    pc -= 4; //undo pc increment to point to current instruction
    pc += offset << 2;
}

void CPU::set_reg(uint8_t reg, uint32_t data)
{
    gpreg_out[reg] = data;
    gpreg_out[0] = 0; // $zero register
}

uint32_t CPU::get_reg(uint8_t reg)
{
    return gpreg_in[reg];
}

void CPU::copy_regs()
{
    //copy output registers to input
    for(int i = 0; i < 32; i++)
    {
        gpreg_in[i] = gpreg_out[i];
    }
}

//INSTRUCTIONS

void CPU::COP0()
{
    if(lookup_cop0.find(ins.rs()) != lookup_cop0.end())
    {
        (this->*lookup_cop0[ins.rs()])();
        return;
    }
    //throw unhandled instruction error
    std::stringstream ss;
    ss << "Unhandled instruction (COP0): " << std::hex << ir;
    throw std::runtime_error(ss.str());
}

void CPU::COP1()
{
    //Not used in PSX
}

void CPU::COP2()
{
}   

void CPU::COP3()
{
    //Not used in PSX
}

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
    set_reg(ins.rt(), ins.imm() << 16);
}

void CPU::ORI() //Bitwise OR Immediate
{
    set_reg(ins.rt(), get_reg(ins.rs()) | ins.imm());
}

void CPU::SW() //Store Word
{
    //if cache is isolated
    if(cop0_status & 0x00010000)
    {
        //TODO: Implement Cache
        std::cout << "Ignoring SW as cache is isolated.\n";
        return;
    }

    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
    {
        offset |= 0xffff0000;
    }
    write32(get_reg(ins.rs()) + offset, get_reg(ins.rt()));
}

void CPU::SLL() //Shift Left Logical
{
    set_reg(ins.rd(), get_reg(ins.rt()) << ins.shamt());
}

void CPU::ADDIU() //Add Immediate Unsigned
{
    set_reg(ins.rt(), get_reg(ins.rs()) + ins.imm());
}

void CPU::J() //Unconditional Jump
{
    pc = (pc & 0xf0000000) | (ins.addr() << 2);
}

void CPU::OR() //Bitwise OR
{
    set_reg(ins.rd(), get_reg(ins.rs()) | get_reg(ins.rt()));
}

void CPU::MTC0() //Move to Coprocessor 0
{
    switch(ins.rd())
    {
        case 12:
            cop0_status = get_reg(ins.rt());
            break;
        case 3:
        case 5:
        case 6:
        case 7:
        case 9:
        case 11:
            //Throw runtime error if value other than 0 is written
            if(get_reg(ins.rt()) != 0)
            {
                std::stringstream ss;
                ss << "Unhandled nonzero write to COP0 register (MTC0): " << ins.rd();
                throw std::runtime_error(ss.str());
            }
            break;
        case 13:
            cop0_cause = get_reg(ins.rt());
            //if nonzero value written, throw error
            if(get_reg(ins.rt()) != 0)
            {
                std::stringstream ss;
                ss << "Unhandled nonzero write to COP0 CAUSE register (MTC0): " << ins.rd();
                throw std::runtime_error(ss.str());
            }
            break;
        default:
            //throw unhandled instruction error
            std::stringstream ss;
            ss << "Unhandled COP0 register (MTC0): " << ins.rd();
            throw std::runtime_error(ss.str());
    }
}

void CPU::BNE() //Branch on Not Equal
{
    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
    {
        offset |= 0xffff0000;
    }
    if(get_reg(ins.rs()) != get_reg(ins.rt()))
    {
        branch(offset);
    }
}

void CPU::ADDI() //Add Immediate
{
    uint32_t extended_imm = ins.imm();
    //pad offset with bit at 16th position
    if(extended_imm & 0x8000)
    {
        extended_imm |= 0xffff0000;
    }
    //check for signed overflow
    //TODO: Handle signed overflow exception
    if(((extended_imm & 0x80000000) && (get_reg(ins.rs()) & 0x80000000) && ((get_reg(ins.rs()) + extended_imm) & 0x80000000 == 0))
        || ((extended_imm & 0x80000000 == 0) && (get_reg(ins.rs()) & 0x80000000 == 0) && ((get_reg(ins.rs()) + extended_imm) & 0x80000000)))
    {
        //throw signed overflow error
        std::stringstream ss;
        ss << "Signed overflow in ADDI: " << std::hex << ir;
        throw std::runtime_error(ss.str());
    }

    set_reg(ins.rt(), get_reg(ins.rs()) + extended_imm);
}

void CPU::LW() // Load Word
{
    pending_load = LoadDelay(ins.rt(), read32(get_reg(ins.rs()) + ins.imm()));
}