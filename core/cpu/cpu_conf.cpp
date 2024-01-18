#include "core/cpu/cpu.hpp"

/**
 * @brief Resets the CPU to its initial state.
 * 
 * Sets all the general purpose registers (except the zero register) to 0xdeadbeef, the HI and LO registers to 0xdeadbeef, the PC to 0xbfc00000, and the coprocessor 0 registers to 0x00000000.
 * 
 */
void CPU::reset()
{
    regs[0] = 0;
    for(int i = 1; i < 32; i++)
        regs[i] = 0xdeadbeef;
    hi = 0xdeadbeef;
    lo = 0xdeadbeef;

    pc = 0xbfc00000;

    cop0_bda = 0x00000000;
    cop0_bpcm = 0x00000000;
    cop0_bpc = 0x00000000;
    cop0_dcic = 0x00000000;
    cop0_bdam = 0x00000000;
    cop0_status = 0x00000000;
    cop0_cause = 0x00000000;

    ins = Instruction(0x00000000);
    ir = 0x00000000;
    ir_next = 0x00000000;
}

/**
 * @brief Configures the instruction lookup table.
 * 
 */
void CPU::conf_ins_lookup()
{
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
}

/**
 * @brief Configures the mnemonic lookup table. (for debugging)
 * 
 */
void CPU::conf_mnemonic_lookup()
{
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