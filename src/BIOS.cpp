#include <fstream>

#include "../include/BIOS.hpp"

/**
 * @brief Construct a new BIOS:: BIOS object
 * 
 * @param path Path to the BIOS file
 * 
 * @throw std::runtime_error If the BIOS size is invalid
 */
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

/**
 * @brief Reads a 32-bit word from the BIOS.
 * 
 * @param offset Offset to read from
 * @return uint32_t Data read
 */
uint32_t BIOS::read32_cpu(uint32_t offset)
{
    //since the system is little endian, we can do this
    return *(uint32_t *)&data[offset];

    //TODO: add compatibility for big endian systems
}

/**
 * @brief Reads a 8-bit word from the BIOS.
 * 
 * @param offset Offset to read from
 * @return uint8_t Data read
 */
uint8_t BIOS::read8_cpu(uint32_t offset)
{
    return data[offset];
}