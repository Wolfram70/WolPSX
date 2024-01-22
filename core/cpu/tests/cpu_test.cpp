#include <cpu_test.hpp>

/**
 * @brief Get the singleton instance
 * 
 * @return RWLog* 
 */
RWLog* RWLog::get_instance()
{
    if(instance == nullptr)
    {
        instance = new RWLog();
    }
    return instance;
}

/**
 * @brief Get the number of reads
 * 
 * @return uint32_t 
 */
uint32_t RWLog::get_read_count()
{
    return readCount;
}

/**
 * @brief Get the number of writes
 * 
 * @return uint32_t 
 */
uint32_t RWLog::get_write_count()
{
    return writeCount;
}

/**
 * @brief Log a read32
 * 
 * @param addr 
 * @param data 
 */
void RWLog::log_read32(uint32_t addr)
{
    RWLogEntry entry(true, addr, 0);
    log.push_back(entry);
    readCount++;
}

/**
 * @brief Log a write32
 * 
 * @param addr 
 * @param data 
 */
void RWLog::log_write32(uint32_t addr, uint32_t data)
{
    RWLogEntry entry(false, addr, data);
    log.push_back(entry);
    writeCount++;
}

/**
 * @brief Log a read16
 * 
 * @param addr 
 * @param data 
 */
void RWLog::log_read16(uint32_t addr)
{
    RWLogEntry entry(true, addr, 0);
    log.push_back(entry);
    readCount++;
}

/**
 * @brief Log a write16
 * 
 * @param addr 
 * @param data 
 */
void RWLog::log_write16(uint32_t addr, uint16_t data)
{
    RWLogEntry entry(false, addr, data);
    log.push_back(entry);
    writeCount++;
}

/**
 * @brief Log a read8
 * 
 * @param addr 
 * @param data 
 */
void RWLog::log_read8(uint32_t addr)
{
    RWLogEntry entry(true, addr, 0);
    log.push_back(entry);
    readCount++;
}

/**
 * @brief Log a write8
 * 
 * @param addr 
 * @param data 
 */
void RWLog::log_write8(uint32_t addr, uint8_t data)
{
    RWLogEntry entry(false, addr, data);
    log.push_back(entry);
    writeCount++;
}

/**
 * @brief Clear the log
 * 
 */
void RWLog::clear()
{
    log.clear();
}

/**
 * @brief Get the entry at the given index
 * 
 * @param index 
 * @return RWLogEntry 
 */
RWLogEntry RWLog::get_entry(int index)
{
    return log[index];
}

/**
 * @brief Get the size of the log
 * 
 * @return int 
 */
int RWLog::size()
{
    return log.size();
}

//REMOVE

#include <core/cpu/cpu.hpp>
#include <iostream>

int main()
{
    CPU cpu;
    cpu.reset();
    cpu.show_regs();
    std::cout << RWLog::get_instance()->get_read_count() << std::endl;
    return 0;
}