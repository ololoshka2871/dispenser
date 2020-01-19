cmake_minimum_required(VERSION 3.13)

set(SOURCE_FILES
    main.cpp
    BoardInit.cpp
    _init.c
    hardfault.cpp
    systick.c
    LedController.cpp

    Button.h
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

#target_include_directories(${PROJECT_NAME} PRIVATE include)
#target_compile_definitions(${PROJECT_NAME} PRIVATE BOARD=BOARD_${TGTNAME})
target_link_libraries(${PROJECT_NAME} PUBLIC
    -Wl,--whole-archive
        STM32_HAL_STM32F042x6
    -Wl,--no-whole-archive
    ws2812b
)

####### some helpers to generate targets
function(make_bin_file target)
    add_custom_command(
        TARGET ${target} POST_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${target} ${target}.bin
    )
endfunction()

function(show_object_size target)
    string(REPLACE "objcopy" "size" CMAKE_OBJSIZE "${CMAKE_OBJCOPY}")
    add_custom_command(
        TARGET ${target} POST_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_OBJSIZE} ${target}
    )
endfunction()

function(add_flash_target target)
        add_custom_target(
                flash-${target} dfu-util -d 1d50:606f -a 0 -R -s 0x08000000 -D ${target}.bin
        )
endfunction()

########################################################################################################################

make_bin_file(${PROJECT_NAME})
show_object_size(${PROJECT_NAME})
add_flash_target(${PROJECT_NAME})
