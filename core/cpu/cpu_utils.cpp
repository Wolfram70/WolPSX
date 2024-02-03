#include <iostream>
#include <sstream>
#include <core/cpu/cpu.hpp>
#include <queue>

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

/**
 * @brief Returns the state of the CPU as a CPUState object.
 * 
 * Ownership of the returned object is transferred to the caller.
 * 
 * @return CPUState* 
 */
CPUState* CPU::get_state(CPUState* cpu_state)
{
    for(int i = 0; i < 32; i++)
    {
        cpu_state->reg_gen[i] = regs[i];
    }
    cpu_state->reg_hi = hi;
    cpu_state->reg_lo = lo;

    cpu_state->program_counter = pc;
    
    cpu_state->reg_cop0_status = cop0_status;
    cpu_state->reg_cop0_bda = cop0_bda;
    cpu_state->reg_cop0_bpc = cop0_bpc;
    cpu_state->reg_cop0_dcic = cop0_dcic;
    cpu_state->reg_cop0_bdam = cop0_bdam;
    cpu_state->reg_cop0_bpcm = cop0_bpcm;
    cpu_state->reg_cop0_cause = cop0_cause;

    cpu_state->ins_current = ins;
    cpu_state->ins_next = Instruction(ir_next);

    cpu_state->load_queue = load_queue;

    return cpu_state;
}

/**
 * @brief Sets the state of the CPU from a CPUState object.
 * 
 * @param cpu_state CPUState object to set the state from
 */
void CPU::set_state(CPUState* cpu_state)
{
    for(int i = 0; i < 32; i++)
    {
        regs[i] = cpu_state->reg_gen[i];
    }
    hi = cpu_state->reg_hi;
    lo = cpu_state->reg_lo;

    pc = cpu_state->program_counter;

    cop0_status = cpu_state->reg_cop0_status;
    cop0_bda = cpu_state->reg_cop0_bda;
    cop0_bpc = cpu_state->reg_cop0_bpc;
    cop0_dcic = cpu_state->reg_cop0_dcic;
    cop0_bdam = cpu_state->reg_cop0_bdam;
    cop0_bpcm = cpu_state->reg_cop0_bpcm;
    cop0_cause = cpu_state->reg_cop0_cause;

    ins = cpu_state->ins_current;
    ir = ins.ins;
    ir_next = cpu_state->ins_next.ins;

    load_queue = cpu_state->load_queue;
}

/**
 * @brief Clocks the CPU without fetching the next instruction.
 * 
 * Used for debugging and testing purposes.
 */
void CPU::clock_nofetch()
{
    decode_and_execute();
    load_regs();
}