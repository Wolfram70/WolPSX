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
#define EXPANSION2_RANGE 0x1f802000, 0x1f802fff

class CPU;
class BIOS;
class RAM;

struct Range
{
    uint32_t start;
    uint32_t end;

    Range(uint32_t start, uint32_t end) : start(start), end(end) {}
    bool contains(uint32_t addr)
    {
        return addr >= start && addr <= end;
    }
    uint32_t offset(uint32_t addr)
    {
        return addr - start;
    }
};

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
    CPU* cpu;
    BIOS* bios;
    RAM *ram;

    Range bios_range = Range(BIOS_RANGE);
    Range mem_ctrl_range = Range(MEM_CTRL_RANGE);
    Range ram_size_range = Range(RAM_SIZE_RANGE);
    Range cache_ctrl_range = Range(CACHE_CTRL_RANGE);
    Range ram_range = Range(RAM_RANGE);
    Range spu_range = Range(SPU_RANGE);
    Range expansion2_range = Range(EXPANSION2_RANGE);
};

#endif