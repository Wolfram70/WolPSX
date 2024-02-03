add_library(test_config INTERFACE)
target_include_directories(test_config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(test_config INTERFACE ${CMAKE_SOURCE_DIR}/include)

add_executable(cpu_arith_tests cpu_arith_tests.cpp cpu_test_rw.cpp cpu_test_util.cpp)
target_link_libraries(cpu_arith_tests PRIVATE test_config)
target_link_libraries(cpu_arith_tests PRIVATE cpu_nrw)

add_test(NAME CPUArithmeticOps COMMAND cpu_arith_tests)
set(failRegex "[.]*Failure([.]*)")
set_property(TEST CPUArithmeticOps PROPERTY FAIL_REGULAR_EXPRESSION "${failRegex}")