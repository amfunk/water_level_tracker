cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(water_level_tracker)

set(PICO_BOARD pico)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# add an executable to the project using the specified source files
add_executable(water_level_tracker
  src/main.c
  )

# add compile definitions to a target
target_compile_definitions(water_level_tracker PRIVATE
  )

# specify include directories to use when compiling a target
target_include_directories(water_level_tracker PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/include
  )

# Add pico_stdlib library which aggregates commonly used features
# Additional libraries should be added space-separated after project name
target_link_libraries(water_level_tracker pico_stdlib)

# enable usb input/output and disable uart input/output
pico_enable_stdio_usb(water_level_tracker 1)
pico_enable_stdio_uart(water_level_tracker 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(water_level_tracker)
