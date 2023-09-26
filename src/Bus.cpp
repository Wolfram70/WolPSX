#include <iostream>
#include <sstream>

#include "../include/Bus.hpp"
#include "../include/BIOS.hpp"
#include "../include/CPU.hpp"
#include "../include/RAM.hpp"

Bus::Bus(std::string bios_path)
{
    cpu = new CPU();
    bios = new BIOS(bios_path);
    ram = new RAM(2 * 1024 * 1024);

    cpu->connectBus(this);
}

uint32_t Bus::read32_cpu(uint32_t addr)
{
    //catch unaligned accesses
    if (addr % 4 != 0)
    {
        //throw a runtime error with the unaligned address converted to hex
        std::stringstream ss;
        ss << "Unaligned read32_cpu: 0x" << std::hex << addr;
        throw std::runtime_error(ss.str());
    }

    if (bios_range.contains(addr))
    {
        return bios->read32_cpu(bios_range.offset(addr));
    }

    //throw a runtime error with the unmapped address converted to hex
    std::stringstream ss;
    ss << "Unmapped address for read32_cpu: 0x" << std::hex << addr;
    throw std::runtime_error(ss.str());
}

void Bus::write32_cpu(uint32_t addr, uint32_t data)
{
    //catch unaligned accesses
    if (addr % 4 != 0)
    {
        //throw a runtime error with the unaligned address converted to hex
        std::stringstream ss;
        ss << "Unaligned write32_cpu: 0x" << std::hex << addr;
        throw std::runtime_error(ss.str());
    }

    if(mem_ctrl_range.contains(addr))
    {
        switch(mem_ctrl_range.offset(addr))
        {
            case 0:
                if(data != 0x1f000000)
                {
                    std::stringstream ss;
                    ss << "Bad Expansion 1 Base Address write: 0x" << std::hex << data;
                    throw std::runtime_error(ss.str());
                }
                break;
            case 4:
                if(data != 0x1f802000)
                {
                    std::stringstream ss;
                    ss << "Bad Expansion 2 Base Address write: 0x" << std::hex << data;
                    throw std::runtime_error(ss.str());
                }
                break;
            default:
                std::stringstream ss;
                ss << "Unhandled write to MEM_CTRL register: " << mem_ctrl_range.offset(addr);
                // throw std::runtime_error(ss.str());
                break;
        }
        return;
    }
    else if(ram_size_range.contains(addr))
    {
        //TODO: implement RAM size register
        return;
    }
    else if(cache_ctrl_range.contains(addr))
    {
        //TODO: implement cache control register
        return;
    }

    //throw a runtime error with the unmapped address converted to hex
    std::stringstream ss;
    ss << "Unmapped address for write32_cpu: 0x" << std::hex << addr;
    throw std::runtime_error(ss.str());
}

void Bus::clock()
{
    cpu->clock();
}