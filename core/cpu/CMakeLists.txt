add_library(cpu 
        cpu.cpp
        cpu_conf.cpp
        ins_cop0.cpp
        ins_cop2.cpp
        ins_special.cpp
        ins.cpp
        cpu_rw.cpp
        cpu_utils.cpp
)

add_library(cpu_nrw 
        cpu.cpp
        cpu_conf.cpp
        ins_cop0.cpp
        ins_cop2.cpp
        ins_special.cpp
        ins.cpp
        cpu_utils.cpp
)

target_link_libraries(cpu PUBLIC compile_options)
target_link_libraries(cpu_nrw PUBLIC compile_options)