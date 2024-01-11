#ifndef BIOS_HPP
#define BIOS_HPP

#include <stdint.h>
#include <string>
#include <vector>

#define BIOS_SIZE 512 * 1024

/**
 * @brief Class to emulate the BIOS.
 * 
 * Implements the BIOS of the PSX.
 */
class BIOS
{
public:
    BIOS(std::string path);
    uint32_t read32_cpu(uint32_t offset);
    uint8_t read8_cpu(uint32_t offset);

private:
    /**
     * @brief Data of the BIOS.
     * 
     */
    std::vector<uint8_t> data;
};

#endif
