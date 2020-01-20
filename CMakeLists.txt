cmake_minimum_required(VERSION 3.13)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi-8-2019-q3-update.cmake)
project(dispenser C CXX ASM)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

set(STM32_FAMILY F0)
set(STM32_FAMILY_LONG STM32F0)
set(STM32_FAMILY_LONG_lx stm32f0xx)
set(STM32_FAMILY_Xl stm32f042x6)
set(MCU STM32F042x6)

find_package(STM32CubeF0 REQUIRED)

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

add_compile_options(
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++14>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
)

add_link_options(
    -mcpu=cortex-m0 -mthumb -O
    -Wall -Wextra -g3
    -nostartfiles -Xlinker --gc-sections --specs=nano.specs --specs=nosys.specs
    -T ${CMAKE_SOURCE_DIR}/ldscripts/ldscriptF042.ld
)

add_definitions(
    -DF_CPU=48000000L
    -DHSE_VALUE=16000000L
    -D${MCU}
    -DWS2812B_NUM_LEDS=1
)


add_subdirectory(libs)
add_subdirectory(src)
