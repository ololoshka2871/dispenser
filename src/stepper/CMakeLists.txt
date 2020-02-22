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

     FreeRunningAccelStepper.cpp
     FreeRunningAccelStepper.h
)

add_library(stepper STATIC ${SOURCES})
target_include_directories(stepper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(stepper PUBLIC STM32_HAL_STM32F042x6)
