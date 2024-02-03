#include <iostream>

#include <core/cpu/cpu.hpp>
#include <cpu_test.hpp>

/**
 * @brief Tests for CPU ADD (SPECIAL 100000)
 * 
 * @param cpu 
 * @return true 
 * @return false 
 */
void test_cpu_add(CPU& cpu)
{
    // Test unsigned addition
    std::cout << "ADD (SPECIAL 100000) Addition: ";
    CPUState test_state;
    test_state.reg_gen[1] = 0x00000001;
    test_state.reg_gen[2] = 0x00000002;
    test_state.ins_current = Instruction(0b00000000010000010001100000100000); // ADD $3, $1, $2
    cpu.set_state(&test_state);
    cpu.clock_nofetch();
    cpu.get_state(&test_state);
    if(test_state.reg_gen[3] == 0x00000003) std::cout << "Success" << std::endl;
    else std::cout << "Failure" << std::endl;
}

int main()
{
    CPU test_cpu;

    test_cpu_add(test_cpu);

    return 0;
}