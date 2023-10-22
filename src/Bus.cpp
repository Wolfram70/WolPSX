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

uint32_t region_mask(uint32_t addr)
{
    switch(addr >> 29)
    {
        case 0:
            return 0x1fffffff; //KUSEG
        case 1:
            return 0x1fffffff; //KUSEG
        case 2:
            return 0x1fffffff; //KUSEG
        case 3:
            return 0x1fffffff; //KUSEG
        case 4:
            return 0x1fffffff; //KSEG0
        case 5:
            return 0x1fffffff; //KSEG1
        case 6:
            return 0xffffffff; //KSEG2
        case 7:
            return 0xffffffff; //KSEG2
        default:
            return 0x00000000; //Should never happen
    }
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

    uint32_t addr_og = addr;
    addr &= region_mask(addr);

    if(bios_range.contains(addr))
    {
        return bios->read32_cpu(bios_range.offset(addr));
    }
    else if(ram_range.contains(addr))
    {
        return ram->read32_cpu(ram_range.offset(addr) & 0x001fffff);
    }

    //throw a runtime error with the unmapped address converted to hex
    std::stringstream ss;
    ss << "Unmapped address for read32_cpu: 0x" << std::hex << addr_og;
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

    uint32_t addr_og = addr;
    addr &= region_mask(addr);

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
                ss << "Unhandled write to MEM_CTRL register: " << addr;
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
    else if(ram_range.contains(addr))
    {
        ram->write32_cpu(ram_range.offset(addr) & 0x001fffff, data);
        return;
    }
    else if(interrupt_range.contains(addr))
    {
        //Implement peripherals and interrupts
        std::cout << "Ignoring write32 to Interrupt Register" << std::endl;
        return;
    }

    //throw a runtime error with the unmapped address converted to hex
    std::stringstream ss;
    ss << "Unmapped address for write32_cpu: 0x" << std::hex << addr_og;
    throw std::runtime_error(ss.str());
}

uint16_t Bus::read16_cpu(uint32_t addr)
{
    //catch unaligned accesses
    if (addr % 2 != 0)
    {
        //throw a runtime error with the unaligned address converted to hex
        std::stringstream ss;
        ss << "Unaligned read16_cpu: 0x" << std::hex << addr;
        throw std::runtime_error(ss.str());
    }

    uint32_t addr_og = addr;
    addr &= region_mask(addr);

    //throw a runtime error with the unmapped address converted to hex
    std::stringstream ss;
    ss << "Unmapped address for read16_cpu: 0x" << std::hex << addr_og;
    throw std::runtime_error(ss.str());
}

void Bus::write16_cpu(uint32_t addr, uint16_t data)
{
    //catch unaligned accesses
    if (addr % 2 != 0)
    {
        //throw a runtime error with the unaligned address converted to hex
        std::stringstream ss;
        ss << "Unaligned write16_cpu: 0x" << std::hex << addr;
        throw std::runtime_error(ss.str());
    }

    uint32_t addr_og = addr;
    addr &= region_mask(addr);

    if(spu_range.contains(addr))
    {
        switch(spu_range.offset(addr))
        {
            default:
                std::stringstream ss;
                ss << "Unhandled write16 to SPU register: " << addr_og;
                // throw std::runtime_error(ss.str());
                break;
        }

        return;
    }

    //throw a runtime error with the unmapped address converted to hex
    std::stringstream ss;
    ss << "Unmapped address for write16_cpu: 0x" << std::hex << addr;
    throw std::runtime_error(ss.str());
}

uint8_t Bus::read8_cpu(uint32_t addr)
{
    uint32_t addr_og = addr;
    addr &= region_mask(addr);

    if(bios_range.contains(addr))
    {
        return bios->read32_cpu(bios_range.offset(addr));
    }
    else if(expansion1_range.contains(addr))
    {
        //TODO: Implement Expansion Region 1
        return 0xff;
    }
    else if(ram_range.contains(addr))
    {
        return ram->read8_cpu(ram_range.offset(addr) & 0x001fffff);
    }

    //throw a runtime error with the unmapped address converted to hex
    std::stringstream ss;
    ss << "Unmapped address for read8_cpu: 0x" << std::hex << addr_og;
    throw std::runtime_error(ss.str());
}

void Bus::write8_cpu(uint32_t addr, uint8_t data)
{
    uint32_t addr_og = addr;
    addr &= region_mask(addr);

    if(expansion2_range.contains(addr))
    {
        std::cout << "Ignoring write8 to Expansion 2" << std::endl;
        return;
    }
    else if(ram_range.contains(addr))
    {
        return ram->write8_cpu(ram_range.offset(addr) & 0x001fffff, data);
    }

    //throw a runtime error with the unmapped address converted to hex
    std::stringstream ss;
    ss << "Unmapped address for write8_cpu: 0x" << std::hex << addr_og;
    throw std::runtime_error(ss.str());
}

void Bus::clock()
{
    cpu->clock();
}