#include <iostream>
#include <sstream>

#include "../include/RAM.hpp"

RAM::RAM(uint32_t size)
{
    data.resize(size);
}

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