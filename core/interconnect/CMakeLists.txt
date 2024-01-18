add_library(interconnect bus.cpp bus_utils.cpp)
target_link_libraries(interconnect PUBLIC compile_options)