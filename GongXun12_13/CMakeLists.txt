cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME GongXun12_13)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
        ./Freertos/Src/croutine.c
        ./Freertos/Src/event_groups.c
        ./Freertos/Src/list.c
        ./Freertos/Src/queue.c
        ./Freertos/Src/stream_buffer.c
        ./Freertos/Src/tasks.c
        ./Freertos/Src/timers.c
        ./Freertos/Port/port.c
        ./Freertos/Port/heap_4.c

        ./Core/Sys/System.c
        ./Core/Sys/Delay.c

        ./Core/Task/led.c
        ./Core/Task/move.c
        ./Core/Task/data.c
        ./Core/Task/show.c
        ./Core/Task/ps2.c

        ./Core/App/PS2/pstwo.c
        ./Core/App/OLED/oled.c
        ./Core/App/Motor/motor.c
        ./Core/App/Anotc/anotc.c
        ./Core/App/Raspi/Raspi.c
        ./Core/App/Servo/servo.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
        ./Freertos
        ./Freertos/Inc
        ./Freertos/Src
        ./Freertos/Port
        ./Core/App
        ./Core/Task
        ./Core/Sys
        ./Core/App/OLED
        ./Core/App/Motor
        ./Core/App/Anotc
        ./Core/App/PS2
        ./Core/App/Raspi
        ./Core/App/Servo
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
