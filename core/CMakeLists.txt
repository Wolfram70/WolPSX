add_library(core INTERFACE)

add_subdirectory(bios)
add_subdirectory(cpu)
add_subdirectory(memory)
add_subdirectory(interconnect)

target_link_libraries(core INTERFACE
    interconnect
    bios
    cpu
    memory
)

target_link_libraries(core INTERFACE compile_options)