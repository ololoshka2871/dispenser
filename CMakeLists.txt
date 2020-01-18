cmake_minimum_required(VERSION 3.13)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi-8-2019-q3-update.cmake)
project(dispenser C CXX ASM)

add_compile_options(
    -mcpu=cortex-m0 -mthumb
    -Wall -Wextra
#   -Werror
    -Wno-deprecated
    -fmessage-length=0
    -fsigned-char
    -ffunction-sections -fdata-sections
    -ffreestanding
    -fno-move-loop-invariants
    -Os -g3
)

add_compile_options($<$<COMPILE_LANGUAGE:C>:-std=gnu11>)

add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++14>)

add_link_options(
    -mcpu=cortex-m0 -mthumb -O
    -Wall -Wextra -g3
    -nostartfiles -Xlinker --gc-sections --specs=nano.specs
    -T ${CMAKE_SOURCE_DIR}/ldscripts/ldscript.ld
)

add_definitions(
    -DF_CPU=48000000L
)


add_subdirectory(libs/STM32_HAL)
add_subdirectory(src)
