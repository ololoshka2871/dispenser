cmake_minimum_required(VERSION 3.13)

set(SOURCES
     AccelStepper.cpp
     AccelStepper.h
     DigitalOut.h
     DigitalOut.cpp
     UsTimer.h
     UsTimer.cpp
     delay.h
     delay.cpp
)

add_library(AccelStepperHAL STATIC ${SOURCES})
target_include_directories(AccelStepperHAL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AccelStepperHAL PUBLIC STM32_HAL_STM32F042x6)
