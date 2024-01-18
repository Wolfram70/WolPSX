#include <iostream>
#include <sstream>
#include <core/cpu/cpu.hpp>

/**
 * @brief Load the next instruction into the instruction register
 * 
 * Executes a read32 at the address given by the program counter and increments the program counter by 4.
 */
void CPU::load_next_ins()
{
    ir = ir_next;
    ir_next = read32(pc);
    ins = Instruction(ir);
    pc += 4;
}

/**
 * @brief Decodes and executes the instruction in the instruction register.
 * 
 * Uses the opcode to lookup the instruction in the opcode lookup table and executes the appropriate function.
 * 
 * @throw std::runtime_error if the instruction is not mapped in the opcode lookup table.
 */
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

/**
 * @brief Branches to the given offset.
 * 
 * The offset is multiplied by 4 before branching.
 * @param offset Offset to branch to
 */
void CPU::branch(uint32_t offset)
{
    pc -= 4; //undo pc increment to point to next instruction
    uint32_t multiplied = offset << 2;
    if((multiplied & 0x80000000) != (offset & 0x80000000))
    {
        std::cout << "MISTAKE!";
    }
    pc += multiplied;
}

/**
 * @brief Sets the value of the given register from the general purpose registers.
 * 
 * @param reg Register to set
 * @param data Value to set the register to
 */
void CPU::set_reg(uint8_t reg, uint32_t data)
{
    // gpreg_out[reg] = data;
    // gpreg_out[0] = 0; // $zero register
    load_queue.push(RegisterLoad(reg, data));
}

/**
 * @brief Gets the value of the given register from the general purpose registers.
 * 
 * @param reg Register to get the value of
 * @return uint32_t Value of the register
 */
uint32_t CPU::get_reg(uint8_t reg)
{
    // return gpreg_in[reg];
    return regs[reg];
}

/**
 * @brief Loads the registers from the load queue.
 * 
 * Decrements the delay of each load in the load queue. If the delay is 0, the data is written to the register.
 */
void CPU::load_regs()
{
    uint8_t num_loads = load_queue.size();
    for(int i = 0; i < num_loads; i++)
    {
        RegisterLoad load = load_queue.front();
        load_queue.pop();
        load.delay--;
        if(load.delay == 0xffffffff)
            regs[load.reg] = load.data;
        else
            load_queue.push(load);
    }
    regs[0] = 0; // $zero register
}

/**
 * @brief Shows the values of the registers.
 * 
 * Used for debugging.
 */
void CPU::show_regs()
{
    std::cout << "Registers:\n";
    for(int i = 0; i < 32; i++)
    {
        std::cout << "R" << i << ": " << std::hex << regs[i] << "\t";
        if(i % 4 == 3)
            std::cout << "\n";
    }
    std::cout << "HI: " << std::hex << hi << "\tLO: " << std::hex << lo << "\n";

}