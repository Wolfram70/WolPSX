#ifndef RAM_H
#define RAM_H

#include <stdint.h>
#include <vector>

class RAM
{
public:
    RAM(uint32_t size);
    uint32_t read32_cpu(uint32_t offset);
    void write32_cpu(uint32_t offset, uint32_t data);
    uint8_t read8_cpu(uint32_t offset);
    void write8_cpu(uint32_t offset, uint8_t data);
private:
    std::vector<uint8_t> data;
};

#endif