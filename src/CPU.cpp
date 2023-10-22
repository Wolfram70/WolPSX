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
    lookup_op[0b101001] = &CPU::SH;
    lookup_op[0b000011] = &CPU::JAL;
    lookup_op[0b001100] = &CPU::ANDI;
    lookup_op[0b101000] = &CPU::SB;
    lookup_op[0b100000] = &CPU::LB;
    lookup_op[0b000100] = &CPU::BEQ;
    lookup_op[0b000111] = &CPU::BGTZ;
    lookup_op[0b000110] = &CPU::BLEZ;
    lookup_op[0b100100] = &CPU::LBU;

    lookup_special[0b000000] = &CPU::SLL;
    lookup_special[0b100101] = &CPU::OR;
    lookup_special[0b101011] = &CPU::STLU;
    lookup_special[0b100001] = &CPU::ADDU;
    lookup_special[0b001000] = &CPU::JR;
    lookup_special[0b100100] = &CPU::AND;
    lookup_special[0b100000] = &CPU::ADD;
    lookup_special[0b001001] = &CPU::JALR;

    lookup_cop0[0b00100] = &CPU::MTC0;
    lookup_cop0[0b00000] = &CPU::MFC0;
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

uint16_t CPU::read16(uint32_t addr)
{
    return bus->read16_cpu(addr);
}

void CPU::write16(uint32_t addr, uint16_t data)
{
    return bus->write16_cpu(addr, data);
}

uint8_t CPU::read8(uint32_t addr)
{
    return bus->read8_cpu(addr);
}

void CPU::write8(uint32_t addr, uint8_t data)
{
    return bus->write8_cpu(addr, data);
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
    // std::cout << "PC: " << std::hex << pc << "\n";
    load_next_ins();
    // std::cout << "Instruction: " << std::hex << ir << "\n";
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
    uint32_t multiplied = offset << 2;
    if(multiplied & 0x80000000 != offset & 0x80000000)
    {
        std::cout << "MISTAKE!";
    }
    pc += multiplied;
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
    uint32_t data_se = ins.imm();
    //pad offset with bit at 16th position
    if(data_se & 0x8000)
    {
        data_se |= 0xffff0000;
    }
    set_reg(ins.rt(), get_reg(ins.rs()) + data_se);
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
    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
    {
        offset |= 0xffff0000;
    }

    pending_load = LoadDelay(ins.rt(), read32(get_reg(ins.rs()) + offset));
}

void CPU::STLU() //Set on Less Than Unsigned
{
    set_reg(ins.rd(), get_reg(ins.rs()) < get_reg(ins.rt()));
}

void CPU::ADDU() //Add Unsigned
{
    set_reg(ins.rd(), get_reg(ins.rs()) + get_reg(ins.rt()));
}

void CPU::SH() //Store Halfword
{
    //if cache is isolated
    if(cop0_status & 0x00010000)
    {
        //TODO: Implement Cache
        std::cout << "Ignoring SH as cache is isolated.\n";
        return;
    }

    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
    {
        offset |= 0xffff0000;
    }
    write16(get_reg(ins.rs()) + offset, get_reg(ins.rt()) & 0xffff);
}

void CPU::JAL() //Jump and Link
{
    uint32_t ra = pc;
    pc = (pc & 0xf0000000) | (ins.addr() << 2);
    set_reg(31, ra);
}

void CPU::ANDI() //Bitwise AND Immediate
{
    set_reg(ins.rt(), get_reg(ins.rs()) & ins.imm());
}

void CPU::SB() //Store Byte
{
    //if cache is isolated
    if(cop0_status & 0x00010000)
    {
        //TODO: Implement Cache
        std::cout << "Ignoring SB as cache is isolated.\n";
        return;
    }

    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
    {
        offset |= 0xffff0000;
    }
    write8(get_reg(ins.rs()) + offset, get_reg(ins.rt()) & 0xff);
}

void CPU::JR() //Jump Register
{
    pc = get_reg(ins.rs());
}

void CPU::LB() //Load Byte
{
    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
    {
        offset |= 0xffff0000;
    }
    uint32_t data_s = read8(get_reg(ins.rs()) + offset);
    if(data_s & 0b10000000)
    {
        data_s |= 0xffffff00;
    }
    pending_load = LoadDelay(ins.rt(), data_s);
}

void CPU::BEQ() //Branch on Equal
{
    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
    {
        offset |= 0xffff0000;
    }
    if(get_reg(ins.rs()) == get_reg(ins.rt()))
    {
        branch(offset);
    }
}

void CPU::MFC0() //Move from Coprocessor 0
{
    switch(ins.rd())
    {
        case 12: //Status
            pending_load = LoadDelay(ins.rt(), cop0_status);
            break;
        case 13: //Cause
            pending_load = LoadDelay(ins.rt(), cop0_cause);
            break;
        default:
            //throw unhandled instruction error
            std::stringstream ss;
            ss << "Unhandled COP0 register (MFC0): " << ins.rd();
            throw std::runtime_error(ss.str());
    }
}

void CPU::AND() //Bitwise AND
{
    set_reg(ins.rd(), get_reg(ins.rs()) & get_reg(ins.rt()));
}

void CPU::ADD() //Add
{
    uint32_t extended_op1 = get_reg(ins.rt());
    uint32_t extended_op2 = get_reg(ins.rs());
    
    //check for signed overflow
    //TODO: Handle signed overflow exception
    if(((extended_op1 & 0x80000000) && (extended_op2 & 0x80000000) && ((extended_op1 + extended_op2) & 0x80000000 == 0))
        || ((extended_op1 & 0x80000000 == 0) && (extended_op2 & 0x80000000 == 0) && ((extended_op1 + extended_op2) & 0x80000000)))
    {
        //throw signed overflow error
        std::stringstream ss;
        ss << "Signed overflow in ADD: " << std::hex << ir;
        throw std::runtime_error(ss.str());
    }

    set_reg(ins.rd(), extended_op1 + extended_op2);
}

void CPU::BGTZ() //Branch on Greater Than Zero
{
    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
    {
        offset |= 0xffff0000;
    }
    if(int32_t(get_reg(ins.rs())) > 0)
    {
        branch(offset);
    }
}

void CPU::BLEZ() //Branch on Less Than or Equal to Zero
{
    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
    {
        offset |= 0xffff0000;
    }
    if(int32_t(get_reg(ins.rs())) <= 0)
    {
        branch(offset);
    }
}

void CPU::LBU() //Load Byte Unsigned
{
    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
    {
        offset |= 0xffff0000;
    }
    uint32_t data = read8(get_reg(ins.rs()) + offset);
    pending_load = LoadDelay(ins.rt(), data);
}

void CPU::JALR() //Jump and Link Register
{
    uint32_t ra = pc;
    pc = get_reg(ins.rs());
    set_reg(ins.rd(), ra);
}