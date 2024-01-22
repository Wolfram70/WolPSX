#ifndef CPU_TEST_HPP
#define CPU_TEST_HPP

#include <cstdint>
#include <vector>

/**
 * @brief Struct to hold a read or write log entry for testing
 * 
 */
struct RWLogEntry
{
    RWLogEntry(bool is_read, uint32_t addr, uint32_t data)
    {
        this->is_read = is_read;
        this->addr = addr;
        this->data = data;
    }

    bool is_read;
    uint32_t addr;
    uint32_t data;
};

/**
 * @brief Class to hold the read/write log for testing
 * 
 */
class RWLog
{
public:
    static RWLog* get_instance();

    uint32_t get_read_count();;
    uint32_t get_write_count();

    void log_read32(uint32_t addr);
    void log_write32(uint32_t addr, uint32_t data);
    void log_read16(uint32_t addr);
    void log_write16(uint32_t addr, uint16_t data);
    void log_read8(uint32_t addr);
    void log_write8(uint32_t addr, uint8_t data);
    
    void clear();
    RWLogEntry get_entry(int index);
    int size();

private:
    RWLog()
    {
        readCount = 0;
        writeCount = 0;
    }

private:
    /**
     * @brief Vector to hold the log entries
     * 
     */
    std::vector<RWLogEntry> log;

    /**
     * @brief Number of reads
     * 
     */
    uint32_t readCount;

    /**
     * @brief Number of writes
     * 
     */
    uint32_t writeCount;

    /**
     * @brief Singleton instance
     * 
     */
    inline static RWLog* instance;
};

#endif