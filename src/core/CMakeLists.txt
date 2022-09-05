add_library(gds_core INTERFACE)

target_include_directories(gds_core INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

add_subdirectory(comms)
add_subdirectory(drive_system)