cmake_minimum_required(VERSION 3.13)

set(SOURCES
    ws2812b.c
)

add_library(ws2812b STATIC ${SOURCES})
target_include_directories(ws2812b PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ws2812b PUBLIC STM32_HAL_STM32F042x6)
