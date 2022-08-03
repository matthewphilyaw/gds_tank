# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS_RELEASE "-O2")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(cmake/pico_sdk_import.cmake)

project(gds_tank C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(gds_tank src/main.c)
add_subdirectory(src)

target_link_libraries(gds_tank pico_stdlib gds_hardware gds_alpha_board)

pico_set_program_name(gds_tank "gds_tank")
pico_set_program_version(gds_tank "0.1")


pico_enable_stdio_uart(gds_tank 1)
pico_enable_stdio_usb(gds_tank 0)

pico_add_extra_outputs(gds_tank)

