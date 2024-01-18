#include <iostream>
#include <sstream>

#include <core/cpu/cpu.hpp>

/**
 * @brief Construct a new CPU object
 * 
 * Sets the initial values of the registers and the initializes the opcode lookup tables.
 * 
 * \b References:
 * @ref reset
 * @ref conf_ins_lookup
 * @ref conf_mnemonic_lookup
 */
CPU::CPU()
{
    reset();
    conf_ins_lookup();
    conf_mnemonic_lookup();
}

/**
 * @brief Clocks the CPU once.
 * 
 * Executes the load_next_ins and decode_and_execute functions. Implements the load delay by copying the output registers to the input registers after the instruction is executed.
 * 
 * \b References:
 * @ref load_next_ins
 * @ref decode_and_execute
 * @ref load_regs
 */
void CPU::clock()
{
    load_next_ins();

    decode_and_execute();

    load_regs();
}

/**
 * @brief Looks up and executes the appropriate coprocessor 1 instruction. (UNUSED)
 * 
 */
void CPU::COP1()
{
    //Not used in PSX
}

/**
 * @brief Looks up and executes the appropriate coprocessor 3 instruction. (UNUSED)
 * 
 */
void CPU::COP3()
{
    //Not used in PSX
}