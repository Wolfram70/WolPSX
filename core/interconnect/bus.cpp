#include <iostream>
#include <sstream>

#include "core/interconnect/bus.hpp"
#include "core/bios/bios.hpp"
#include "core/cpu/cpu.hpp"
#include "core/memory/ram.hpp"

/**
 * @brief Construct a new Bus:: Bus object
 * 
 * @param bios_path Path to the BIOS file
 * 
 * \b References:
 * @ref CPU::CPU
 * @ref BIOS::BIOS
 * @ref RAM::RAM
 * @ref CPU::connectBus
 */
Bus::Bus(std::string bios_path)
{
    cpu = new CPU();
    bios = new BIOS(bios_path);
    ram = new RAM(2 * 1024 * 1024);

    cpu->connectBus(this);
}

/**
 * @brief Reads a 32-bit word from the given address
 * 
 * TODO: Map all addresses.
 * 
 * @param addr Address to read from
 * @return uint32_t Data read from the address
 * 
 * @throw std::runtime_error If the address is unaligned
 * @throw std::runtime_error If the address is unmapped
 * 
 * \b References:
 * @ref BIOS::read32_cpu
 * @ref RAM::read32_cpu
 * @ref Range::contains
 * @ref Range::offset
 * @ref region_mask
 */
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
        return ram->read32_cpu(ram_range.offset(addr));
    }
    else if(interrupt_range.contains(addr))
    {
        std::cout << "Unhandled Interrupt Control Read: Just returning 0" << std::endl;
        // TODO: Implement interrupts
        return 0;
    }

    //throw a runtime error with the unmapped address converted to hex
    std::stringstream ss;
    ss << "Unmapped address for read32_cpu: 0x" << std::hex << addr_og;
    throw std::runtime_error(ss.str());
}

/**
 * @brief Writes a 32-bit word to the given address
 * 
 * TODO: Map all addresses.
 * 
 * @param addr Address to write to
 * @param data Data to write to the address
 * 
 * @throw std::runtime_error If the address is unaligned
 * @throw std::runtime_error If the address is unmapped
 * @throw std::runtime_error If the data written to any of the MEM_CTRL registers is invalid
 * 
 * \b References:
 * @ref RAM::write32_cpu
 * @ref Range::contains
 * @ref Range::offset
 * @ref region_mask
 */
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
        ram->write32_cpu(ram_range.offset(addr), data);
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

/**
 * @brief Reads a 16-bit word from the given address
 * 
 * TODO: Map all addresses.
 * 
 * @param addr Address to read from
 * @return uint16_t Data read from the address
 * 
 * @throw std::runtime_error If the address is unaligned
 * @throw std::runtime_error If the address is unmapped
 * 
 * \b References:
 * @ref Range::contains
 * @ref Range::offset
 * @ref region_mask
 */
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

/**
 * @brief Writes a 16-bit word to the given address
 * 
 * TODO: Map all addresses.
 * 
 * @param addr Address to write to
 * @param data Data to write to the address
 * 
 * @throw std::runtime_error If the address is unaligned
 * @throw std::runtime_error If the address is unmapped
 * 
 * \b References:
 * @ref Range::contains
 * @ref Range::offset
 * @ref region_mask
 */
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
        //TODO: Implement SPU
        switch(spu_range.offset(addr))
        {
            default:
                std::stringstream ss;
                ss << "Unhandled write16 (" << data << ") to SPU register: " << addr_og;
                // throw std::runtime_error(ss.str());
                break;
        }

        return;
    }
    else if(timer_range.contains(addr))
    {
        //TODO implement timer
        std::cout << "Ignoring write16 to Timer Register" << std::endl;
        return;
    }

    //throw a runtime error with the unmapped address converted to hex
    std::stringstream ss;
    ss << "Unmapped address for write16_cpu: 0x" << std::hex << addr;
    throw std::runtime_error(ss.str());
}

/**
 * @brief Reads a 8-bit word from the given address
 * 
 * TODO: Implement Expansion Region 1.
 * TODO: Map all addresses.
 * 
 * @param addr Address to read from
 * @return uint8_t Data read from the address
 * 
 * @throw std::runtime_error If the address is unmapped
 * 
 * \b References:
 * @ref BIOS::read32_cpu
 * @ref RAM::read8_cpu
 * @ref Range::contains
 * @ref Range::offset
 * @ref region_mask
 */
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
        return ram->read8_cpu(ram_range.offset(addr));
    }

    //throw a runtime error with the unmapped address converted to hex
    std::stringstream ss;
    ss << "Unmapped address for read8_cpu: 0x" << std::hex << addr_og;
    throw std::runtime_error(ss.str());
}

/**
 * @brief Writes a 8-bit word to the given address
 *
 * TODO: Implement Expansion Region 2.
 * TODO: Map all addresses.
 * 
 * @param addr Address to write to
 * @param data Data to write to the address
 * 
 * @throw std::runtime_error If the address is unmapped
 * 
 * \b References:
 * @ref RAM::write8_cpu
 * @ref Range::contains
 * @ref Range::offset
 * @ref region_mask
 */
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
        return ram->write8_cpu(ram_range.offset(addr), data);
    }

    //throw a runtime error with the unmapped address converted to hex
    std::stringstream ss;
    ss << "Unmapped address for write8_cpu: 0x" << std::hex << addr_og;
    throw std::runtime_error(ss.str());
}