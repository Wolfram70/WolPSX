cmake_minimum_required(VERSION 3.15)

project(WolPSX)

add_library(compile_options INTERFACE)
target_compile_features(compile_options INTERFACE cxx_std_17)
target_include_directories(compile_options INTERFACE include)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU>")
set(msvc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

target_compile_options(compile_options INTERFACE
    "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wpedantic;-Werror>>"
    "$<${msvc_like_cxx}:$<BUILD_INTERFACE:-W3;-WX>>"
)

add_subdirectory(core)
add_executable(WolPSX main.cpp)
target_link_libraries(WolPSX PRIVATE compile_options core)
