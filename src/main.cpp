#include "../include/Bus.hpp"

int main()
{
    Bus bus("test/bios/SCPH1001.BIN");
    while(true)
        bus.clock();
    return 0;
}