#ifndef CPU_HPP
#define CPU_HPP

#include <stdint.h>
#include <map>
#include <string>
#include <queue>

class Bus;

/**
 * @brief Structure to access different parts of an instruction by value
 * 
 */
struct Instruction
{
    uint32_t ins;

    /**
     * @brief Construct a new Instruction object
     * 
     */
    Instruction() {}

    /**
     * @brief Construct a new Instruction object
     * 
     * @param ins Instruction in the form of a 32-bit unsigned integer
     */
    Instruction(uint32_t ins) : ins(ins) {}

    /**
     * @brief Opcode of the instruction. Size: 6 bits [31-26]
     * 
     * @return uint32_t Opcode
     */
    uint32_t opcode() { return ins >> 26; }

    /**
     * @brief Source register for the instruction. Size: 5 bits [25-21]
     * 
     * @return uint32_t Register ID
     */
    uint32_t rs() { return (ins >> 21) & 0x1f; }

    /**
     * @brief Target register for the instruction. Size: 5 bits [20-16]
     * 
     * @return uint32_t Register ID
     */
    uint32_t rt() { return (ins >> 16) & 0x1f; }

    /**
     * @brief Destination register for the instruction. Size: 5 bits [15-11]
     * 
     * @return uint32_t Register ID
     */
    uint32_t rd() { return (ins >> 11) & 0x1f; }

    /**
     * @brief Shift amount for the instruction. Size: 5 bits [10-6]
     * 
     * Used in the case of shift instructions.
     * @return uint32_t Shift amount
     */
    uint32_t shamt() { return (ins >> 6) & 0x1f; }

    /**
     * @brief Function field for the instruction. Size: 6 bits [5-0]
     * 
     * Used to lookup the functions in case of 'SPECIAL' instructions where the opcode is 0b000000.
     * @return uint32_t Function ID
     */
    uint32_t funct() { return ins & 0x3f; }

    /**
     * @brief Immediate value for the instruction. Size: 16 bits [15-0]
     * 
     * @return uint32_t Immediate argument
     */
    uint32_t imm() { return ins & 0xffff; } // Bits 15-0

    /**
     * @brief Address value for jump/branch instructions. Size: 26 bits [25-0]
     * 
     * @return uint32_t Address
     */
    uint32_t addr() { return ins & 0x3ffffff; } // Bits 25-0
};

/**
 * @brief Struture to store details of loads to the general purpose registers.
 * 
 */
struct RegisterLoad
{
    /**
     * @brief Register ID
     * 
     */
    uint32_t reg;

    /**
     * @brief Data to be loaded
     * 
     */
    uint32_t data;

    /**
     * @brief Delay in clock cycles
     * 
     */
    uint32_t delay;

    /**
     * @brief Construct a new RegisterLoad object
     * 
     */
    RegisterLoad() {}

    /**
     * @brief Construct a new RegisterLoad object with no delay
     * 
     * @param reg Register ID
     * @param data Data to be loaded
     */
    RegisterLoad(uint32_t reg, uint32_t data): reg(reg), data(data) { delay = 0; }

    /**
     * @brief Construct a new RegisterLoad object with delay
     * 
     * @param reg Register ID
     * @param data Data to be loaded
     * @param delay Delay
     */
    RegisterLoad(uint32_t reg, uint32_t data, uint32_t delay): reg(reg), data(data), delay(delay) {}
};

/**
 * @brief Structure to store and transfer the state of the CPU for debugging purposes.
 * 
 * All the register values, instruction details and load queue are stored in this structure.
 */
struct CPUState
{
    uint32_t reg_gen[32];
    uint32_t reg_hi;
    uint32_t reg_lo;
    uint32_t program_counter;
    uint32_t reg_cop0_status;
    uint32_t reg_cop0_bda;
    uint32_t reg_cop0_bpc;
    uint32_t reg_cop0_dcic;
    uint32_t reg_cop0_bdam;
    uint32_t reg_cop0_bpcm;
    uint32_t reg_cop0_cause;

    Instruction ins_current;
    Instruction ins_next;

    std::queue<RegisterLoad> load_queue;
};

/**
 * @brief Class to emulate the CPU.
 * 
 * Implements the CPU of the PSX (The MIPS R3000A CPU).
 */
class CPU
{
public:
    CPU();

    /**
     * @brief Connects Bus to the CPU.
     * 
     * Used by the constructor of Bus to connect the CPU to the Bus.
     * @param bus Pointer to the bus structure
     */
    void connectBus(Bus* bus) { this->bus = bus; }

    void clock();

private:
    void load_next_ins();
    void decode_and_execute();

    uint32_t read32(uint32_t addr);
    void write32(uint32_t addr, uint32_t data);
    uint16_t read16(uint32_t addr);
    void write16(uint32_t addr, uint16_t data);
    uint8_t read8(uint32_t addr);
    void write8(uint32_t addr, uint8_t data);

private:
    /**
     * @brief Pointer to the Bus object
     * 
     */
    Bus* bus;

    /**
     * @brief Queue to store the loads to the general purpose registers.
     * 
     * Used to implement load delay.
     */
    std::queue<RegisterLoad> load_queue;

    /**
     * @brief Program counter
     * 
     */
    uint32_t pc = 0;

    /**
     * @brief Instruction register
     * 
     */
    uint32_t ir;

    /**
     * @brief Instruction immediately after the current instruction in the memory.
     * 
     */
    uint32_t ir_next;

    /**
     * @brief Instruction in the form of the Instruction structure.
     * 
     */
    Instruction ins;

    /**
     * @brief General purpose registers.
     * 
     */
    uint32_t regs[32] = {0xdeaddeed};

    /**
     * @brief HI register.
     * 
     * Used to store the higher 32 bits of the result of multiplication and remainder of division.
     */
    uint32_t hi = 0;

    /**
     * @brief LO register.
     * 
     * Used to store the lower 32 bits of the result of multiplication and quotient of division.
     */
    uint32_t lo = 0;

    /**
     * @brief COP0 status register
     * 
     * Used to store the status of the CPU.
     */
    uint32_t cop0_status;

    /**
     * @brief COP0 breakpoint exception register
     * 
     * Generates an exception when the value of the register is equal to the value of the PC.
     */
    uint32_t cop0_bpc;

    /**
     * @brief COP0 breakpoint exception register (data)
     * 
     * Generates an exception when address is accessed as a data load/store instead of instruction fetch.
     */
    uint32_t cop0_bda;

    /**
     * @brief COP0 breakpoint exception register (hardware)
     * 
     * TODO: Figure out what exactly this does.
     */
    uint32_t cop0_dcic;

    /**
     * @brief Bitmask applied to COP0 breakpoint exception register (data)
     * 
     * Used to trigger exception on a range of addresses.
     */
    uint32_t cop0_bdam;

    /**
     * @brief Bitmask applied on COP0 breakpoint exception register.
     * 
     */
    uint32_t cop0_bpcm;

    /**
     * @brief COP0 cause register
     * 
     * Used to store the cause of the exception and is read-only. Apparently only [9:8] bits are writable to force an exception.
     */
    uint32_t cop0_cause;

private:
    void branch(uint32_t offset);
    void set_reg(uint8_t reg, uint32_t data);
    uint32_t get_reg(uint8_t reg);
    void load_regs();
    void conf_ins_lookup();
    void conf_mnemonic_lookup();

public:
    void show_regs();
    void reset();
    CPUState* get_state();

private:
    /**
     * @brief Lookup table for instructions
     * 
     */
    std::map<uint8_t, void (CPU::*)()> lookup_op;

    /**
     * @brief Lookup table for special instructions (opcode = 0b000000)
     * 
     */
    std::map<uint8_t, void (CPU::*)()> lookup_special;

    /**
     * @brief Lookup table for cop0 instructions (opcode = 0b010000)
     * 
     */
    std::map<uint8_t, void (CPU::*)()> lookup_cop0;

    /**
     * @brief Lookup table for cop1 instructions (opcode = 0b010001)
     * 
     */
    std::map<uint8_t, void (CPU::*)()> lookup_cop2;

    /**
     * @brief Lookup table for the mnemonics of instructions.
     * 
     */
    std::map<uint8_t, std::string> lookup_mnemonic_op;

    /**
     * @brief Lookup table for the mnemonics of special instructions.
     * 
     */
    std::map<uint8_t, std::string> lookup_mnemonic_special;

    void LUI();
    void ORI();
    void SW();
    void ADDIU();
    void J();
    void BNE();
    void ADDI();
    void LW();
    void SH();
    void JAL();
    void ANDI();
    void SB();
    void LB();
    void BEQ();
    void BGTZ();
    void BLEZ();
    void LBU();
    void SLTI();
    void SLTIU();

    void SPECIAL();
    void SLL();
    void OR();
    void SLTU();
    void ADDU();
    void JR();
    void AND();
    void ADD();
    void JALR();
    void SRA();
    void SUBU();
    void DIV();
    void MFLO();
    void SRL();
    void DIVU();
    void MFHI();
    void SLT();

    void BLGE();
    void BLTZ();
    void BGEZ();
    void BGEZAL();
    void BLTZAL();

    void COP0();
    void MTC0();
    void MFC0();

    void COP1();

    void COP2();

    void COP3();
};

#endif