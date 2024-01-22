#include <cstdint>
#include <core/cpu/cpu.hpp>
#include <cpu_test.hpp>

/**
 * @brief Dummy read32 for testing
 * 
 * @param addr 
 * @return uint32_t 
 */
uint32_t CPU::read32(uint32_t addr)
{
    RWLog::get_instance()->log_read32(addr);
    return 0xdeadc0de;
}

/**
 * @brief Dummy write32 for testing
 * 
 * @param addr 
 * @param data 
 */
void CPU::write32(uint32_t addr, uint32_t data)
{
    RWLog::get_instance()->log_write32(addr, data);
    return;
}

/**
 * @brief Dummy read16 for testing
 * 
 * @param addr 
 * @return uint16_t 
 */
uint16_t CPU::read16(uint32_t addr)
{
    RWLog::get_instance()->log_read16(addr);
    return 0xc0de;
}

/**
 * @brief Dummy write16 for testing
 * 
 * @param addr 
 * @param data 
 */
void CPU::write16(uint32_t addr, uint16_t data)
{
    RWLog::get_instance()->log_write16(addr, data);
    return;
}

/**
 * @brief Dummy read8 for testing
 * 
 * @param addr 
 * @return uint8_t 
 */
uint8_t CPU::read8(uint32_t addr)
{
    RWLog::get_instance()->log_read8(addr);
    return 0xde;
}

/**
 * @brief Dummy write8 for testing
 * 
 * @param addr 
 * @param data 
 */
void CPU::write8(uint32_t addr, uint8_t data)
{
    RWLog::get_instance()->log_write8(addr, data);
    return;
}