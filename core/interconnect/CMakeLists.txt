add_library(interconnect bus.cpp bus_utils.cpp)
target_link_libraries(interconnect PRIVATE compile_options)