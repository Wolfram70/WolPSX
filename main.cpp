#include <string>
#include <iostream>

#include <core/interconnect/bus.hpp>

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <bios_path>" << std::endl;
        return 1;
    }
    std::string bios_path = argv[1];
    Bus bus(bios_path);
    while(true)
        bus.clock();
    return 0;
} 