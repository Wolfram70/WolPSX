#ifndef BUS_HPP
#define BUS_HPP

#include <stdint.h>
#include <string>

#define BIOS_RANGE 0xbfc00000, 0xbfc7ffff
#define MEM_CTRL_RANGE 0x1f801000, 0x1f801023
#define RAM_SIZE_RANGE 0x1f801060, 0x1f801063
#define CACHE_CTRL_RANGE 0xfffe0130, 0xfffe0133

class CPU;
class BIOS;

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

    void clock();

private:
    CPU* cpu;
    BIOS* bios;

    Range bios_range = Range(BIOS_RANGE);
    Range mem_ctrl_range = Range(MEM_CTRL_RANGE);
    Range ram_size_range = Range(RAM_SIZE_RANGE);
    Range cache_ctrl_range = Range(CACHE_CTRL_RANGE);
};

#endif