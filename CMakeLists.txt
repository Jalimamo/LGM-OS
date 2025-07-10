cmake_minimum_required(VERSION 3.30)
project(LGM_OS)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(LGM_OS
    gdt.cpp
    gdt.h
    interrupts.cpp
    interrupts.h
    kernel.cpp
    port.cpp
    port.h
    types.h)
