#include <core/cpu/cpu.hpp>
#include <core/interconnect/bus.hpp>

/**
 * @brief Read a 32 bit word from the bus.
 * 
 * @param addr Address to read from
 * @return uint32_t Data read from the bus
 * 
 * \b References:
 * @ref Bus::read32_cpu
 */
uint32_t CPU::read32(uint32_t addr)
{
    return bus->read32_cpu(addr);
}

/**
 * @brief Write a 32 bit word to the bus.
 * 
 * @param addr Address to write to
 * @param data Data to write to the bus
 * 
 * \b References:
 * @ref Bus::write32_cpu
 */
void CPU::write32(uint32_t addr, uint32_t data)
{
    bus->write32_cpu(addr, data);
}

/**
 * @brief Read a 16 bit word from the bus.
 * 
 * @param addr Address to read from
 * @return uint16_t Data read from the bus
 * 
 * \b References:
 * @ref Bus::read16_cpu
 */
uint16_t CPU::read16(uint32_t addr)
{
    return bus->read16_cpu(addr);
}

/**
 * @brief Write a 16 bit word to the bus.
 * 
 * @param addr Address to write to
 * @param data Data to write to the bus
 * 
 * \b References
 * @ref Bus::write16_cpu
 */
void CPU::write16(uint32_t addr, uint16_t data)
{
    return bus->write16_cpu(addr, data);
}

/**
 * @brief Read a 8 bit word from the bus.
 * 
 * @param addr Address to read from
 * @return uint8_t Data read from the bus
 * 
 * \b References
 * @ref Bus::read8_cpu
 */
uint8_t CPU::read8(uint32_t addr)
{
    return bus->read8_cpu(addr);
}

/**
 * @brief Write a 8 bit word to the bus.
 * 
 * @param addr Address to write to
 * @param data Data to write to the bus
 * 
 * \b References
 * @ref Bus::write8_cpu
 */
void CPU::write8(uint32_t addr, uint8_t data)
{
    return bus->write8_cpu(addr, data);
}