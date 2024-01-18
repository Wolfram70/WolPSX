#include <core/interconnect/bus.hpp>
#include <core/cpu/cpu.hpp>

/**
 * @brief Returns the region mask for a given address
 * 
 * The regions are: \n
 * - \b KUSEG: 0x00000000 - 0x7fffffff \n
 * - \b KSEG0: 0x80000000 - 0x9fffffff \n
 * - \b KSEG1: 0xa0000000 - 0xbfffffff \n
 * - \b KSEG2: 0xc0000000 - 0xffffffff \n
 * @param addr Address to get the region mask for
 * @return uint32_t Region mask
 */
uint32_t Bus::region_mask(uint32_t addr)
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

/**
 * @brief Clocks the PSX
 * 
 * Clocks all the components of the PSX and serves as a synchronization point between the components.
 * 
 * @ref CPU::clock
 */
void Bus::clock()
{
    cpu->clock();
}