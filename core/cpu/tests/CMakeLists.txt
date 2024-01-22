add_executable(cpu_tests cpu_test_rw.cpp cpu_test.cpp)
target_include_directories(cpu_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(cpu_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(cpu_tests PRIVATE cpu_nrw)