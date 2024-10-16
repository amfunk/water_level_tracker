cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(<PROJECT_NAME_HERE>)

set(PICO_BOARD <BOARD_NAME_HERE>)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# add an executable to the project using the specified source files
add_executable(<PROJECT_NAME_HERE>
  src/main.c
  )

# add compile definitions to a target
target_compile_definitions(<PROJECT_NAME_HERE> PRIVATE
  )

# specify include directories to use when compiling a target
target_include_directories(<PROJECT_NAME_HERE> PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/include
  )

# Add pico_stdlib library which aggregates commonly used features
# Additional libraries should be added space-separated after project name
target_link_libraries(<PROJECT_NAME_HERE> pico_stdlib)

# enable usb input/output and disable uart input/output
pico_enable_stdio_usb(<PROJECT_NAME_HERE> 1)
pico_enable_stdio_uart(<PROJECT_NAME_HERE> 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(<PROJECT_NAME_HERE>)
