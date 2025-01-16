cmake_minimum_required(VERSION 3.13)

# This MUST happen before include() as of SDK 2.0.0
set(PICO_BOARD pico)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(water_level_tracker)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# add an executable to the project using the specified source files
add_executable(${CMAKE_PROJECT_NAME}
  src/main.c
  )

# add compile definitions to a target
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
  )

# specify include directories to use when compiling a target
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/include
  )

# Add pico_stdlib library which aggregates commonly used features
# Additional libraries should be added space-separated after project name
target_link_libraries(${CMAKE_PROJECT_NAME} pico_stdlib hardware_adc hardware_gpio pico_multicore pico_aon_timer)

# enable usb input/output and disable uart input/output
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
