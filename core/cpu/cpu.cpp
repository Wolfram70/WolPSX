#include <iostream>
#include <sstream>

#include "core/cpu/cpu.hpp"
#include "core/interconnect/bus.hpp"

/**
 * @brief Construct a new CPU object
 * 
 * Sets the initial values of the registers and the initializes the opcode lookup tables.
 */
CPU::CPU()
{
    regs[0] = 0; //$zero register
    pc = 0xbfc00000;

    cop0_bda = 0x00000000;
    cop0_bpcm = 0x00000000;
    cop0_bpc = 0x00000000;
    cop0_dcic = 0x00000000;
    cop0_bdam = 0x00000000;
    cop0_status = 0x00000000;
    cop0_cause = 0x00000000;

    hi = 0xdeaddeed;
    lo = 0xdeaddeed;

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
    lookup_op[0b000001] = &CPU::BLGE;
    lookup_op[0b001010] = &CPU::SLTI;
    lookup_op[0b001011] = &CPU::SLTIU;

    lookup_special[0b000000] = &CPU::SLL;
    lookup_special[0b100101] = &CPU::OR;
    lookup_special[0b101011] = &CPU::SLTU;
    lookup_special[0b100001] = &CPU::ADDU;
    lookup_special[0b001000] = &CPU::JR;
    lookup_special[0b100100] = &CPU::AND;
    lookup_special[0b100000] = &CPU::ADD;
    lookup_special[0b001001] = &CPU::JALR;
    lookup_special[0b000011] = &CPU::SRA;
    lookup_special[0b100011] = &CPU::SUBU;
    lookup_special[0b011010] = &CPU::DIV;
    lookup_special[0b010010] = &CPU::MFLO;
    lookup_special[0b000010] = &CPU::SRL;
    lookup_special[0b011011] = &CPU::DIVU;
    lookup_special[0b010000] = &CPU::MFHI;
    lookup_special[0b101010] = &CPU::SLT;

    lookup_cop0[0b00100] = &CPU::MTC0;
    lookup_cop0[0b00000] = &CPU::MFC0;

    lookup_mnemonic_op[0b000000] = "SPECIAL";
    lookup_mnemonic_op[0b010000] = "COP0";
    lookup_mnemonic_op[0b010001] = "COP1";
    lookup_mnemonic_op[0b010010] = "COP2";
    lookup_mnemonic_op[0b010011] = "COP3";
    lookup_mnemonic_op[0b001111] = "LUI";
    lookup_mnemonic_op[0b001101] = "ORI";
    lookup_mnemonic_op[0b101011] = "SW";
    lookup_mnemonic_op[0b001001] = "ADDIU";
    lookup_mnemonic_op[0b000010] = "J";
    lookup_mnemonic_op[0b000101] = "BNE";
    lookup_mnemonic_op[0b001000] = "ADDI";
    lookup_mnemonic_op[0b100011] = "LW";
    lookup_mnemonic_op[0b101001] = "SH";
    lookup_mnemonic_op[0b000011] = "JAL";
    lookup_mnemonic_op[0b001100] = "ANDI";
    lookup_mnemonic_op[0b101000] = "SB";
    lookup_mnemonic_op[0b100000] = "LB";
    lookup_mnemonic_op[0b000100] = "BEQ";
    lookup_mnemonic_op[0b000111] = "BGTZ";
    lookup_mnemonic_op[0b000110] = "BLEZ";
    lookup_mnemonic_op[0b100100] = "LBU";
    lookup_mnemonic_op[0b000001] = "BLGE";
    lookup_mnemonic_op[0b001010] = "SLTI";
    lookup_mnemonic_op[0b001011] = "SLTIU";

    lookup_mnemonic_special[0b000000] = "SLL";
    lookup_mnemonic_special[0b000000] = "SLL";
    lookup_mnemonic_special[0b100101] = "OR";
    lookup_mnemonic_special[0b101011] = "SLTU";
    lookup_mnemonic_special[0b100001] = "ADDU";
    lookup_mnemonic_special[0b001000] = "JR";
    lookup_mnemonic_special[0b100100] = "AND";
    lookup_mnemonic_special[0b100000] = "ADD";
    lookup_mnemonic_special[0b001001] = "JALR";
    lookup_mnemonic_special[0b000011] = "SRA";
    lookup_mnemonic_special[0b100011] = "SUBU";
    lookup_mnemonic_special[0b011010] = "DIV";
    lookup_mnemonic_special[0b010010] = "MFLO";
    lookup_mnemonic_special[0b000010] = "SRL";
    lookup_mnemonic_special[0b011011] = "DIVU";
    lookup_mnemonic_special[0b010000] = "MFHI";
    lookup_mnemonic_special[0b101010] = "SLT";
}

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
 * @brief Read a 32 bit word from the bus.
 * 
 * @param addr Address to read from
 * @return uint32_t Data read from the bus
 * 
 * \b References:
 * @ref Bus::read32_cpu
 */
uint32_t CPU::read32(uint32_t addr)
{
    return bus->read32_cpu(addr);
}

/**
 * @brief Write a 32 bit word to the bus.
 * 
 * @param addr Address to write to
 * @param data Data to write to the bus
 * 
 * \b References:
 * @ref Bus::write32_cpu
 */
void CPU::write32(uint32_t addr, uint32_t data)
{
    bus->write32_cpu(addr, data);
}

/**
 * @brief Read a 16 bit word from the bus.
 * 
 * @param addr Address to read from
 * @return uint16_t Data read from the bus
 * 
 * \b References:
 * @ref Bus::read16_cpu
 */
uint16_t CPU::read16(uint32_t addr)
{
    return bus->read16_cpu(addr);
}

/**
 * @brief Write a 16 bit word to the bus.
 * 
 * @param addr Address to write to
 * @param data Data to write to the bus
 * 
 * \b References
 * @ref Bus::write16_cpu
 */
void CPU::write16(uint32_t addr, uint16_t data)
{
    return bus->write16_cpu(addr, data);
}

/**
 * @brief Read a 8 bit word from the bus.
 * 
 * @param addr Address to read from
 * @return uint8_t Data read from the bus
 * 
 * \b References
 * @ref Bus::read8_cpu
 */
uint8_t CPU::read8(uint32_t addr)
{
    return bus->read8_cpu(addr);
}

/**
 * @brief Write a 8 bit word to the bus.
 * 
 * @param addr Address to write to
 * @param data Data to write to the bus
 * 
 * \b References
 * @ref Bus::write8_cpu
 */
void CPU::write8(uint32_t addr, uint8_t data)
{
    return bus->write8_cpu(addr, data);
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
 * @brief Clocks the CPU once.
 * 
 * Executes the load_next_ins and decode_and_execute functions. Implements the load delay by copying the output registers to the input registers after the instruction is executed.
 * 
 * \b References:
 * @ref load_next_ins
 * @ref decode_and_execute
 * @ref load_regs
 * @ref set_reg
 */
void CPU::clock()
{
    // std::cout << "PC: " << std::hex << pc - 4 << "\t"; //REMOVE
    load_next_ins();
    // std::cout << "Ins: " << std::hex << ir << "\t"; //REMOVE
    // std::cout << "Mnem: " << lookup_mnemonic_op[ins.opcode()] << "\t"; //REMOVE
    if(ins.opcode() == 0b000000)
    {
        // std::cout << "Func: " << lookup_mnemonic_special[ins.funct()] << "\t"; //REMOVE
    }
    // std::cout << std::endl; //REMOVE

    decode_and_execute();

    load_regs();
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

//INSTRUCTIONS

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
 * @brief Looks up and executes the appropriate coprocessor 1 instruction. (UNUSED)
 * 
 */
void CPU::COP1()
{
    //Not used in PSX
}

/**
 * @brief Looks up and executes the appropriate coprocessor 2 (Graphics) instruction.
 * 
 */
void CPU::COP2()
{
}   

/**
 * @brief Looks up and executes the appropriate coprocessor 3 instruction. (UNUSED)
 * 
 */
void CPU::COP3()
{
    //Not used in PSX
}

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