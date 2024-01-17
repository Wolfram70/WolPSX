#ifndef RAM_H
#define RAM_H

#include <stdint.h>
#include <vector>

/**
 * @brief Class to emulate the RAM.
 * 
 * Implements the RAM of the PSX.
 */
class RAM
{
public:
    RAM(uint32_t size);
    uint32_t read32_cpu(uint32_t offset);
    void write32_cpu(uint32_t offset, uint32_t data);
    uint16_t read16_cpu(uint32_t offset);
    void write16_cpu(uint32_t offset, uint16_t data);
    uint8_t read8_cpu(uint32_t offset);
    void write8_cpu(uint32_t offset, uint8_t data);
private:

    /**
     * @brief Data of the RAM.
     * 
     */
    std::vector<uint8_t> data;
};

#endif