#ifndef CPU_HPP
#define CPU_HPP

#include <stdint.h>
#include <map>

class Bus;

/**
 * @brief Structure to access different parts of an instruction by value
 * 
 */
struct Instruction
{
    uint32_t ins;

    Instruction() {}
    Instruction(uint32_t ins) : ins(ins) {}

    /**
     * @brief Opcode of the instruction. Size: 6 bits [31-26]
     * 
     * @return uint32_t 
     */
    uint32_t opcode() { return ins >> 26; }

    /**
     * @brief Source register for the instruction. Size: 5 bits [25-21]
     * 
     * @return uint32_t 
     */
    uint32_t rs() { return (ins >> 21) & 0x1f; }

    /**
     * @brief Target register for the instruction. Size: 5 bits [20-16]
     * 
     * @return uint32_t 
     */
    uint32_t rt() { return (ins >> 16) & 0x1f; }

    /**
     * @brief Destination register for the instruction. Size: 5 bits [15-11]
     * 
     * @return uint32_t 
     */
    uint32_t rd() { return (ins >> 11) & 0x1f; }

    /**
     * @brief Shift amount for the instruction. Size: 5 bits [10-6]
     * 
     * @return uint32_t 
     */
    uint32_t shamt() { return (ins >> 6) & 0x1f; }

    /**
     * @brief Function field for the instruction. Used to look up operation in the case of 'special' instructions (Opcode = 0h00). Size: 6 bits [5-0]
     * 
     * @return uint32_t 
     */
    uint32_t funct() { return ins & 0x3f; }

    /**
     * @brief Immediate value for the instruction. Size: 16 bits [15-0]
     * 
     * @return uint32_t 
     */
    uint32_t imm() { return ins & 0xffff; } // Bits 15-0

    /**
     * @brief Address value for jump/branch instructions. Size: 26 bits [25-0]
     * 
     * @return uint32_t 
     */
    uint32_t addr() { return ins & 0x3ffffff; } // Bits 25-0
};

/**
 * @brief Stores details of load to implement load delay
 * 
 */
struct LoadDelay
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

    LoadDelay() {}
    LoadDelay(uint32_t a, uint32_t b) {reg = a; data = b;}
};

class CPU
{
public:
    CPU();
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
    Bus* bus;

    LoadDelay pending_load;

    uint32_t pc = 0; // Program counter
    uint32_t ir = 0;
    uint32_t ir_next = 0;
    Instruction ins;
    uint32_t gpreg_in[32] = {0xdeaddeed}; // General purpose registers (input - for load delay slot)
    uint32_t gpreg_out[32] = {0xdeaddeed}; // General purpose registers (output - for load delay slot)
    uint32_t hi = 0; // Mult/Div registers (higher 32 bits)
    uint32_t lo = 0; // Mult/Div registers (lower 32 bits)

    //COP0 registers
    uint32_t cop0_status;
    uint32_t cop0_bpc;
    uint32_t cop0_bda;
    uint32_t cop0_dcic;
    uint32_t cop0_bdam;
    uint32_t cop0_bpcm;
    uint32_t cop0_cause;

private:
    std::map<uint8_t, void (CPU::*)()> lookup_op; // Lookup table for instructions
    std::map<uint8_t, void (CPU::*)()> lookup_special; // Lookup table for special instructions
    std::map<uint8_t, void (CPU::*)()> lookup_cop0; // Lookup table for cop0 instructions
    std::map<uint8_t, void (CPU::*)()> lookup_cop2; // Lookup table for cop1 instructions

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

    //Help
    void branch(uint32_t offset);
    void set_reg(uint8_t reg, uint32_t data);
    uint32_t get_reg(uint8_t reg);
    void copy_regs();
};

#endif