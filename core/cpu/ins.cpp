#include <core/cpu/cpu.hpp>
#include <iostream>
#include <sstream>

/**
 * @brief Load Upper Immediate
 * 
 * \b References:
 * @ref set_reg
 * @ref Instruction::rt
 * @ref Instruction::imm
 */
void CPU::LUI()
{
    set_reg(ins.rt(), ins.imm() << 16);
}

/**
 * @brief Bitwise OR Immediate
 * 
 * \b References:
 * @ref set_reg
 * @ref get_reg
 * @ref Instruction::rs
 * @ref Instruction::imm
 */
void CPU::ORI() //Bitwise OR Immediate
{
    set_reg(ins.rt(), get_reg(ins.rs()) | ins.imm());
}

/**
 * @brief Store Word
 * 
 * TODO: Implement Cache
 * 
 * \b References:
 * @ref set_reg
 * @ref get_reg
 * @ref write_32
 * @ref Instruction::rt
 * @ref Instruction::rs
 * @ref cop0_status
 */
void CPU::SW()
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

/**
 * @brief Add Immediate Unsigned
 * 
 * \b References:
 * @ref set_reg
 * @ref get_reg
 * @ref Instruction::rt
 * @ref Instruction::rs
 * @ref Instruction::imm
 * 
 */
void CPU::ADDIU()
{
    uint32_t data_se = ins.imm();
    //pad offset with bit at 16th position
    if(data_se & 0x8000)
    {
        data_se |= 0xffff0000;
    }
    set_reg(ins.rt(), get_reg(ins.rs()) + data_se);

    // std::cout << "Added " << std::hex << data_se << " to " << get_reg(ins.rs()) << " to get " << get_reg(ins.rt()) << "\n"; //REMOVE
}

/**
 * @brief Unconditional Jump
 * 
 * \b References:
 * @ref Instruction::addr
 */
void CPU::J()
{
    pc = ((pc - 4) & 0xf0000000) | (ins.addr() << 2);
}

/**
 * @brief Branch on Not Equal
 * 
 * \b References:
 * @ref Instruction::imm
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref get_reg
 * @ref branch
 */
void CPU::BNE()
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

/**
 * @brief Add Immediate
 * 
 * Signed addition.
 * 
 * @throw std::runtime_error if signed overflow occurs.
 * 
 * \b References:
 * @ref Instruction::imm
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref get_reg
 * @ref set_reg
 */
void CPU::ADDI()
{
    uint32_t extended_imm = ins.imm();
    //pad offset with bit at 16th position
    if(extended_imm & 0x8000)
    {
        extended_imm |= 0xffff0000;
    }
    //check for signed overflow
    //TODO: Handle signed overflow exception
    if(((extended_imm & 0x80000000) && (get_reg(ins.rs()) & 0x80000000) && (((get_reg(ins.rs()) + extended_imm) & 0x80000000) == 0))
        || (((extended_imm & 0x80000000) == 0) && ((get_reg(ins.rs()) & 0x80000000) == 0) && ((get_reg(ins.rs()) + extended_imm) & 0x80000000)))
    {
        //throw signed overflow error
        std::stringstream ss;
        ss << "Signed overflow in ADDI: " << std::hex << ir;
        throw std::runtime_error(ss.str());
    }

    set_reg(ins.rt(), get_reg(ins.rs()) + extended_imm);
}

/**
 * @brief Load Word
 * 
 * Sign extends the immediate value read from memory.
 * 
 * \b References:
 * @ref Instruction::imm
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref get_reg
 * @ref read32
 * @ref RegisterLoad
 * 
 */
void CPU::LW()
{
    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
    {
        offset |= 0xffff0000;
    }

    load_queue.push(RegisterLoad(ins.rt(), read32(get_reg(ins.rs()) + offset), 1));
}

/**
 * @brief Store Halfword
 * 
 * TODO: Implement Cache
 * 
 * \b References:
 * @ref Instruction::imm
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref get_reg
 * @ref write16
 * @ref cop0_status
 * 
 */
void CPU::SH()
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

/**
 * @brief Jump and Link
 * 
 * Stores the address of the next instruction in register 31 and jumps to the address given by the instruction.
 * 
 * \b References:
 * @ref Instruction::addr
 * @ref set_reg
 * 
 */
void CPU::JAL()
{
    // std::cout << "Jumped to: " << std::hex << ((pc & 0xf0000000) | (ins.addr() << 2)) << "\n"; //REMOVE
    // uint32_t ra = pc + 4;
    uint32_t ra = pc;
    pc = ((pc - 4) & 0xf0000000) | (ins.addr() << 2);
    set_reg(31, ra);
}

/**
 * @brief Bitwise AND Immediate
 * 
 * \b References:
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref Instruction::imm
 * @ref get_reg
 * @ref set_reg
 * 
 */
void CPU::ANDI()
{
    uint32_t data = ins.imm() & get_reg(ins.rs());
    set_reg(ins.rt(), data);
}

/**
 * @brief Store Byte
 *
 * TODO: Implement Cache
 * 
 * \b References:
 * @ref Instruction::imm
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref get_reg
 * @ref write8
 * @ref cop0_status
 * 
 */
void CPU::SB()
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

/**
 * @brief Load Byte
 * 
 * Sign extends the byte read from memory.
 * 
 * \b References:
 * @ref Instruction::imm
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref get_reg
 * @ref read8
 * @ref RegisterLoad
 * 
 */
void CPU::LB()
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

    load_queue.push(RegisterLoad(ins.rt(), data_s, 1));
}

/**
 * @brief Branch on Equal
 * 
 * \b References:
 * @ref Instruction::imm
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref get_reg
 * @ref branch
 * 
 */
void CPU::BEQ()
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

/**
 * @brief Branch on Greater Than Zero
 * 
 * \b References:
 * @ref Instruction::imm
 * @ref Instruction::rs
 * @ref get_reg
 * @ref branch
 * 
 */
void CPU::BGTZ()
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

/**
 * @brief Branch on Less Than or Equal to Zero
 * 
 * \b References:
 * @ref Instruction::imm
 * @ref Instruction::rs
 * @ref get_reg
 * @ref branch
 * 
 */
void CPU::BLEZ()
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

/**
 * @brief Load Byte Unsigned
 * 
 * Unsigned load. Zero extends the byte read from memory.
 * 
 * \b References:
 * @ref Instruction::imm
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref get_reg
 * @ref read8
 * @ref RegisterLoad
 * 
 */
void CPU::LBU()
{
    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
    {
        offset |= 0xffff0000;
    }
    uint32_t data = read8(get_reg(ins.rs()) + offset);
    load_queue.push(RegisterLoad(ins.rt(), data, 1));
}

/**
 * @brief Choose between BLTZAL, BGEZAL, BLTZ, BGEZ
 * 
 * \b References:
 * @ref BLTZAL
 * @ref BGEZAL
 * @ref BLTZ
 * @ref BGEZ
 * 
 */
void CPU::BLGE() //Choose between BLTZAL, BGEZAL, BLTZ, BGEZ
{
    //check if the 16th bit of the instruction is set
    if(ir & 0x00010000)
    {
        //check if the 20th bit of the instruction is set
        if(ir & 0x00100000)
            BGEZAL();
        else
            BGEZ();
    }
    else
    {
        //check if the 20th bit of the instruction is set
        if(ir & 0x00100000)
            BLTZAL();
        else
            BLTZ();
    }
}

/**
 * @brief Branch on Less Than Zero
 * 
 * \b References:
 * @ref Instruction::imm
 * @ref Instruction::rs
 * @ref get_reg
 * @ref branch
 * 
 */
void CPU::BLTZ()
{
    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
        offset |= 0xffff0000;
    int32_t op1 = get_reg(ins.rs());
    uint8_t val = op1 < 0;
    if(val)
        branch(offset);
}

/**
 * @brief Branch on Less Than Zero And Link
 * 
 * \b References:
 * @ref Instruction::imm
 * @ref Instruction::rs
 * @ref get_reg
 * @ref branch
 * @ref set_reg
 * 
 */
void CPU::BLTZAL()
{
    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
        offset |= 0xffff0000;
    int32_t op1 = get_reg(ins.rs());
    uint8_t val = op1 < 0;
    if(val)
    {
        // uint32_t ra = pc + 4;
        uint32_t ra = pc;
        branch(offset);
        set_reg(31, ra);
    }
}

/**
 * @brief Branch on Greater Than or Equal to Zero
 * 
 * \b References:
 * @ref Instruction::imm
 * @ref Instruction::rs
 * @ref get_reg
 * @ref branch
 * 
 */
void CPU::BGEZ()
{
    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
        offset |= 0xffff0000;
    int32_t op1 = get_reg(ins.rs());
    uint8_t val = op1 >= 0;
    if(val)
        branch(offset);
}

/**
 * @brief Branch on Greater Than or Equal to Zero And Link
 * 
 * \b References:
 * @ref Instruction::imm
 * @ref Instruction::rs
 * @ref get_reg
 * @ref branch
 * @ref set_reg
 * 
 */
void CPU::BGEZAL()
{
    uint32_t offset = ins.imm();
    //pad offset with bit at 16th position
    if(offset & 0x8000)
        offset |= 0xffff0000;
    int32_t op1 = get_reg(ins.rs());
    uint8_t val = op1 >= 0;
    if(val)
    {
        // uint32_t ra = pc + 4;
        uint32_t ra = pc;
        branch(offset);
        set_reg(31, ra);
    }
}

/**
 * @brief Set on Less Than Immediate
 * 
 * \b References:
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref Instruction::imm
 * @ref get_reg
 * @ref set_reg
 * 
 */
void CPU::SLTI()
{
    uint32_t imm_se = ins.imm();
    //pad offset with bit at 16th position
    if(imm_se & 0x8000)
        imm_se |= 0xffff0000;
    int32_t op1 = get_reg(ins.rs());
    int32_t op2 = imm_se;
    set_reg(ins.rt(), op1 < op2);
}

/**
 * @brief Set on Less Than Immediate Unsigned
 * 
 * \b References:
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref Instruction::imm
 * @ref get_reg
 * @ref set_reg
 * 
 */
void CPU::SLTIU()
{
    uint32_t arg = ins.imm();
    //pad argument with bit at 16th position
    if(arg & 0x8000)
        arg |= 0xffff0000;

    uint32_t val = (get_reg(ins.rs()) < arg);
    set_reg(ins.rt(), val);
}