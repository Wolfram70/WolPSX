#include <fstream>

#include "../include/BIOS.hpp"

BIOS::BIOS(std::string path)
{
    //Make data a vector of size 512KB
    data.resize(512 * 1024);

    //Open the file
    std::ifstream file(path, std::ios::binary);

    //Check file size
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    if (fileSize != BIOS_SIZE)
    {
        throw std::runtime_error("Invalid BIOS size");
    }

    //Read the file
    file.seekg(0, std::ios::beg);
    file.read((char *)data.data(), fileSize);
    file.close();
}

uint32_t BIOS::read32_cpu(uint32_t offset)
{
    //since the system is little endian, we can do this
    return *(uint32_t *)&data[offset];

    //TODO: add compatibility for big endian systems
}

uint8_t BIOS::read8_cpu(uint32_t offset)
{
    return data[offset];
}