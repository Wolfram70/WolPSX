#include <core/cpu/cpu.hpp>
#include <sstream>
#include <iostream>

/**
 * @brief Looks up and executes the appropriate coprocessor 1 instruction.
 * 
 * @throw std::runtime_error if the instruction is not mapped in the lookup_cop0 table.
 */
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

/**
 * @brief Move to Coprocessor 0
 * 
 * @throw std::runtime_error if nonzero value is written to any register other than status. (TODO: Handle these cases)
 * @throw std::runtime_error if an unhandled register is written to. (Not one of the following: status, cause, bda, bpcm, bpc, dcic, bdam).
 * 
 * \b References:
 * @ref Instruction::rd
 * @ref Instruction::rt
 * @ref cop0_status
 * @ref cop0_cause
 * @ref get_reg
 * @ref set_reg
 */
void CPU::MTC0()
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
            // cop0_cause = get_reg(ins.rt());
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

/**
 * @brief Move From Coprocessor 0
 * 
 * @throw std::runtime_error if an unhandled register is read from. (Not one of the following: status, cause).
 * 
 * \b References:
 * @ref Instruction::rd
 * @ref Instruction::rt
 * @ref cop0_status
 * @ref cop0_cause
 * @ref set_reg
 * @ref RegisterLoad
 * 
 */
void CPU::MFC0()
{
    switch(ins.rd())
    {
        case 12: //Status
            load_queue.push(RegisterLoad(ins.rt(), cop0_status, 1));
            break;
        case 13: //Cause
            break;
        default:
            //throw unhandled instruction error
            std::stringstream ss;
            ss << "Unhandled COP0 register (MFC0): " << ins.rd();
            throw std::runtime_error(ss.str());
    }
}