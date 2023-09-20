#ifndef BIOS_HPP
#define BIOS_HPP

#include <stdint.h>
#include <string>
#include <vector>

#define BIOS_SIZE 512 * 1024

class BIOS
{
public:
    BIOS(std::string path);
    uint32_t read32_cpu(uint32_t offset);

private:
    std::vector<uint8_t> data;
};

#endif
