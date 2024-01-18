#include <iostream>
#include <sstream>

#include "core/memory/ram.hpp"

/**
 * @brief Construct a new RAM:: RAM object
 * 
 * Initializes the RAM with 0xca.
 * 
 * @param size Size of the RAM in bytes
 */
RAM::RAM(uint32_t size)
{
    data = std::vector<uint8_t>(size, 0xca);
}

/**
 * @brief Reads a 32-bit word from the RAM.
 * 
 * @param offset Offset to read from
 * @return uint32_t Data read
 * 
 * @throw std::runtime_error If the offset is unaligned
 * @throw std::runtime_error If the offset is out of bounds
 */
uint32_t RAM::read32_cpu(uint32_t offset)
{
    if(offset >= data.size())
    {
        std::stringstream ss;
        ss << "Size exceeded for read32_cpu (RAM): 0x" << std::hex << offset;
        throw std::runtime_error(ss.str());
    }
    if(offset % 4 != 0)
    {
        std::stringstream ss;
        ss << "Unaligned read32_cpu (RAM): 0x" << std::hex << offset;
        throw std::runtime_error(ss.str());
    }
    return *(uint32_t*)&data[offset];
}

/**
 * @brief Writes a 32-bit word to the RAM.
 * 
 * @param offset Offset to write to
 * @param data Data to write
 * 
 * @throw std::runtime_error If the offset is unaligned
 * @throw std::runtime_error If the offset is out of bounds
 */
void RAM::write32_cpu(uint32_t offset, uint32_t data)
{
    if(offset >= this->data.size())
    {
        std::stringstream ss;
        ss << "Size exceeded for write32_cpu (RAM): 0x" << std::hex << offset;
        throw std::runtime_error(ss.str());
    }
    if(offset % 4 != 0)
    {
        std::stringstream ss;
        ss << "Unaligned write32_cpu (RAM): 0x" << std::hex << offset;
        throw std::runtime_error(ss.str());
    }
    *(uint32_t*)&this->data[offset] = data;
}

/**
 * @brief Reads a byte from the RAM.
 * 
 * @param offset Offset to read from
 * @return uint16_t Data read
 * 
 * @throw std::runtime_error If the offset is out of bounds
 */
uint8_t RAM::read8_cpu(uint32_t offset)
{
    if(offset >= data.size())
    {
        std::stringstream ss;
        ss << "Size exceeded for read8_cpu (RAM): 0x" << std::hex << offset;
        throw std::runtime_error(ss.str());
    }
    return data[offset];
}

/**
 * @brief Writes a byte to the RAM.
 * 
 * @param offset Offset to write to
 * @param data Data to write
 * 
 * @throw std::runtime_error If the offset is out of bounds
 */
void RAM::write8_cpu(uint32_t offset, uint8_t data)
{
    if(offset >= this->data.size())
    {
        std::stringstream ss;
        ss << "Size exceeded for write8_cpu (RAM): 0x" << std::hex << offset;
        throw std::runtime_error(ss.str());
    }
    this->data[offset] = data;
}

/**
 * @brief Reads a 16-bit word from the RAM.
 * 
 * @param offset Offset to read from
 * @return uint16_t Data read
 * 
 * @throw std::runtime_error If the offset is unaligned
 * @throw std::runtime_error If the offset is out of bounds
 */
uint16_t RAM::read16_cpu(uint32_t offset)
{
    if(offset >= data.size())
    {
        std::stringstream ss;
        ss << "Size exceeded for read16_cpu (RAM): 0x" << std::hex << offset;
        throw std::runtime_error(ss.str());
    }
    if(offset % 2 != 0)
    {
        std::stringstream ss;
        ss << "Unaligned read16_cpu (RAM): 0x" << std::hex << offset;
        throw std::runtime_error(ss.str());
    }
    return *(uint16_t*)&data[offset];
}

/**
 * @brief Writes a 16-bit word to the RAM.
 * 
 * @param offset Offset to write to
 * @param data Data to write
 * 
 * @throw std::runtime_error If the offset is unaligned
 * @throw std::runtime_error If the offset is out of bounds
 */
void RAM::write16_cpu(uint32_t offset, uint16_t data)
{
    if(offset >= this->data.size())
    {
        std::stringstream ss;
        ss << "Size exceeded for write16_cpu (RAM): 0x" << std::hex << offset;
        throw std::runtime_error(ss.str());
    }
    if(offset % 2 != 0)
    {
        std::stringstream ss;
        ss << "Unaligned write16_cpu (RAM): 0x" << std::hex << offset;
        throw std::runtime_error(ss.str());
    }
    *(uint16_t*)&this->data[offset] = data;
}