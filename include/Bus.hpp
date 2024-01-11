#ifndef BUS_HPP
#define BUS_HPP

#include <stdint.h>
#include <string>

#define BIOS_RANGE 0x1fc00000, 0x1fc7ffff
#define MEM_CTRL_RANGE 0x1f801000, 0x1f801023
#define RAM_SIZE_RANGE 0x1f801060, 0x1f801063
#define CACHE_CTRL_RANGE 0xfffe0130, 0xfffe0133
#define RAM_RANGE 0x00000000, 0x007fffff
#define SPU_RANGE 0x1f801c00, 0x1f801ffc
#define EXPANSION2_RANGE 0x1f802000, 0x1f803fff
#define EXPANSION1_RANGE 0x1f000000, 0x1f7fffff
#define INTERRUPT_RANGE 0x1f801070, 0x1f801077

class CPU;
class BIOS;
class RAM;

/**
 * @brief Structure to store a range of addresses to allow easy checking.
 * 
 */
struct Range
{
    /**
     * @brief Start of the range (inclusive)
     * 
     */
    uint32_t start;

    /**
     * @brief End of the range (inclusive)
     * 
     */
    uint32_t end;

    /**
     * @brief Construct a new Range object
     * 
     * @param start Start of the range (inclusive)
     * @param end End of the range (inclusive)
     */
    Range(uint32_t start, uint32_t end) : start(start), end(end) {}

    /**
     * @brief Checks if the given address is in the range
     * 
     * @param addr Address to check
     * @return true Address is in the range
     * @return false Address is not in the range
     */
    bool contains(uint32_t addr)
    {
        return addr >= start && addr <= end;
    }

    /**
     * @brief Gets the offset of the given address from the start of the range
     * 
     * @param addr Address to get the offset of
     * @return uint32_t Offset of the address from the start of the range
     */
    uint32_t offset(uint32_t addr)
    {
        return addr - start;
    }
};

/**
 * @brief Class to implement the Bus.
 * 
 * Implements the Bus of the PSX. This is the central class that glues all the other components together. All communication between components (classes) is done through this class.
 */
class Bus
{
public:
    Bus(std::string bios_path);

    uint32_t read32_cpu(uint32_t addr);
    void write32_cpu(uint32_t addr, uint32_t data);

    uint16_t read16_cpu(uint32_t addr);
    void write16_cpu(uint32_t addr, uint16_t data);

    uint8_t read8_cpu(uint32_t addr);
    void write8_cpu(uint32_t addr, uint8_t data);

    void clock();

private:
    uint32_t region_mask(uint32_t addr);

private:
    /**
     * @brief Pointer to the CPU object
     * 
     */
    CPU* cpu;

    /**
     * @brief Pointer to the BIOS object
     * 
     */
    BIOS* bios;

    /**
     * @brief Pointer to the RAM object
     * 
     */
    RAM *ram;

    /**
     * @brief Range of the BIOS
     * 
     */
    Range bios_range = Range(BIOS_RANGE);

    /**
     * @brief Range of the Memory Control Registers
     * 
     */
    Range mem_ctrl_range = Range(MEM_CTRL_RANGE);

    /**
     * @brief Range of the RAM Size Register
     * 
     */
    Range ram_size_range = Range(RAM_SIZE_RANGE);

    /**
     * @brief Range of the Cache Control Register
     * 
     */
    Range cache_ctrl_range = Range(CACHE_CTRL_RANGE);

    /**
     * @brief Range of the RAM
     * 
     */
    Range ram_range = Range(RAM_RANGE);

    /**
     * @brief Range of the SPU (Sound Processing Unit) Registers
     * 
     */
    Range spu_range = Range(SPU_RANGE);

    /**
     * @brief Range of the Expansion 2
     * 
     */
    Range expansion2_range = Range(EXPANSION2_RANGE);

    /**
     * @brief Range of the Expansion 1
     * 
     */
    Range expansion1_range = Range(EXPANSION1_RANGE);

    /**
     * @brief Range of the Interrupt Registers
     * 
     */
    Range interrupt_range = Range(INTERRUPT_RANGE);
};

#endif