#include <sstream>

#include "core/cpu/cpu.hpp"

/**
 * @brief Looks up and executes the appropriate SPECIAL instruction.
 * 
 * @throw std::runtime_error if the instruction is not mapped in the lookup_special table.
 */
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

/**
 * @brief Shift Left Logical
 * 
 * \b References:
 * @ref set_reg
 * @ref Instruction::rd
 * @ref Instruction::rt
 * @ref Instruction::shamt
 */
void CPU::SLL()
{
    set_reg(ins.rd(), get_reg(ins.rt()) << ins.shamt());
}

/**
 * @brief Bitwise OR
 * 
 * \b References:
 * @ref set_reg
 * @ref get_reg
 * @ref Instruction::rd
 * @ref Instruction::rs
 * @ref Instruction::rt
 */
void CPU::OR()
{
    set_reg(ins.rd(), get_reg(ins.rs()) | get_reg(ins.rt()));
}

/**
 * @brief Set on Less Than Unsigned
 * 
 * \b References:
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref Instruction::rd
 * @ref get_reg
 * @ref set_reg
 * 
 */
void CPU::SLTU()
{
    set_reg(ins.rd(), get_reg(ins.rs()) < get_reg(ins.rt()));
}

/**
 * @brief Add Unsigned
 * 
 * \b References:
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref Instruction::rd
 * @ref get_reg
 * @ref set_reg
 * 
 */
void CPU::ADDU()
{
    set_reg(ins.rd(), get_reg(ins.rs()) + get_reg(ins.rt()));
}

/**
 * @brief Jump Register
 * 
 * \b References:
 * @ref Instruction::rs
 * @ref get_reg
 * 
 */
void CPU::JR()
{
    // std::cout << "Jumped to: " << std::hex << get_reg(ins.rs()) << "\n"; //REMOVE
    pc = get_reg(ins.rs());
}

/**
 * @brief Bitwise AND
 * 
 * \b References:
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref Instruction::rd
 * @ref get_reg
 * @ref set_reg
 * 
 */
void CPU::AND()
{
    set_reg(ins.rd(), get_reg(ins.rs()) & get_reg(ins.rt()));
}

/**
 * @brief Add
 * 
 * Signed addition.
 * TODO: Handle signed overflow exception
 * 
 * @throw std::runtime_error if signed overflow occurs.
 * 
 * \b References:
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref Instruction::rd
 * @ref get_reg
 * @ref set_reg
 * 
 */
void CPU::ADD()
{
    uint32_t extended_op1 = get_reg(ins.rt());
    uint32_t extended_op2 = get_reg(ins.rs());
    
    //check for signed overflow
    //TODO: Handle signed overflow exception
    if(((extended_op1 & 0x80000000) && (extended_op2 & 0x80000000) && (((extended_op1 + extended_op2) & 0x80000000) == 0))
        || (((extended_op1 & 0x80000000) == 0) && ((extended_op2 & 0x80000000) == 0) && ((extended_op1 + extended_op2) & 0x80000000)))
    {
        //throw signed overflow error
        std::stringstream ss;
        ss << "Signed overflow in ADD: " << std::hex << ir;
        throw std::runtime_error(ss.str());
    }

    set_reg(ins.rd(), extended_op1 + extended_op2);
}

/**
 * @brief Jump and Link Register
 * 
 * Use a register (specified by the instruction in rd) to store the address of the next instruction and jump to the address given by the register specified by the instruction in rs.
 * 
 * \b References:
 * @ref Instruction::rs
 * @ref Instruction::rd
 * @ref set_reg
 * 
 */
void CPU::JALR()
{
    // uint32_t ra = pc + 4;
    uint32_t ra = pc;
    pc = get_reg(ins.rs());
    set_reg(ins.rd(), ra);
}

/**
 * @brief Shift Right Arithmetic
 * 
 * \b References:
 * @ref Instruction::rt
 * @ref Instruction::shamt
 * @ref get_reg
 * @ref set_reg
 * 
 */
void CPU::SRA()
{
    uint32_t data = get_reg(ins.rt());
    uint32_t shamt = ins.shamt();
    uint32_t sign_bit = data & 0x80000000;
    for(uint32_t i = 0; i < shamt; i++)
    {
        data >>= 1;
        data |= sign_bit;
    }
    set_reg(ins.rd(), data);
}

/**
 * @brief Subtract Unsigned
 * 
 * \b References:
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref Instruction::rd
 * @ref get_reg
 * @ref set_reg
 * 
 */
void CPU::SUBU()
{
    set_reg(ins.rd(), get_reg(ins.rs()) - get_reg(ins.rt()));
}

/**
 * @brief Divide
 * 
 * @throw std::runtime_error if division by zero occurs.
 * 
 * \b References:
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref get_reg
 * @ref lo
 * @ref hi
 * 
 */
void CPU::DIV()
{
    uint32_t op1 = get_reg(ins.rs());
    uint32_t op2 = get_reg(ins.rt());
    if(op2 == 0)
    {
        //throw division by zero error
        if(int32_t(op1) < 0)
        {
            lo = 1;
            hi = op1;
        }
        else
        {
            lo = 0xffffffff;
            hi = op1;
        }
        std::stringstream ss;
        ss << "Division by zero in DIV: " << std::hex << ir;
        throw std::runtime_error(ss.str());
    }
    if(op1 == 0x80000000 && op2 == 0xffffffff)
    {
        lo = 0x80000000;
        hi = 0;
        return;
    }
    lo = int32_t(op1) / int32_t(op2);
    hi = int32_t(op1) % int32_t(op2);
}

/**
 * @brief Move From LO
 * 
 * TODO: Stall until the mul/div is complete.
 * 
 * \b References:
 * @ref Instruction::rd
 * @ref set_reg
 * @ref lo
 * 
 */
void CPU::MFLO()
{
    set_reg(ins.rd(), lo);
}

/**
 * @brief Shift Right Logical
 * 
 * \b References:
 * @ref Instruction::rt
 * @ref Instruction::shamt
 * @ref get_reg
 * @ref set_reg
 * 
 */
void CPU::SRL()
{
    uint32_t data = get_reg(ins.rt()) >> ins.shamt();
    set_reg(ins.rd(), data);
}

/**
 * @brief Divide Unsigned
 * 
 * @throw std::runtime_error if division by zero occurs.
 * 
 * \b References:
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref get_reg
 * @ref lo
 * @ref hi
 * 
 */
void CPU::DIVU()
{
    uint32_t op1 = get_reg(ins.rs());
    uint32_t op2 = get_reg(ins.rt());

    if(op2 == 0)
    {
        //throw division by zero error
        lo = 0xffffffff;
        hi = op1;
        std::stringstream ss;
        ss << "Division by zero in DIVU: " << std::hex << ir;
        throw std::runtime_error(ss.str());
    }

    lo = op1 / op2;
    hi = op1 % op2;
}

/**
 * @brief Move From HI
 * 
 * TODO: Stall until the mul/div is complete.
 * 
 * \b References:
 * @ref Instruction::rd
 * @ref set_reg
 * @ref hi
 * 
 */
void CPU::MFHI()
{
    set_reg(ins.rd(), hi);
}

/**
 * @brief Set on Less Than
 * 
 * \b References:
 * @ref Instruction::rs
 * @ref Instruction::rt
 * @ref Instruction::rd
 * @ref get_reg
 * @ref set_reg
 * 
 */
void CPU::SLT()
{
    int32_t op1 = get_reg(ins.rs());
    int32_t op2 = get_reg(ins.rt());
    set_reg(ins.rd(), op1 < op2);
}