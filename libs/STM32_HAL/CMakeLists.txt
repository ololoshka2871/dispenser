cmake_minimum_required(VERSION 3.13)

SET(STM32_DRIVERS_BASE        ${STM32CubeF0}/Drivers)
SET(HAL_BASE                  ${STM32_DRIVERS_BASE}/${STM32_FAMILY_LONG}xx_HAL_Driver)
SET(HAL_BASE_SRC_DIR          ${HAL_BASE}/Src)
SET(CMSIS_BASE                ${STM32_DRIVERS_BASE}/CMSIS)

SET(HAL_SRC
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_adc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_adc_ex.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_can.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_cec.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_cortex.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_crc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_dac.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_dac_ex.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_dma.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_eth.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_flash.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_flash_ex.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_gpio.c   
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_hcd.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_i2c.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_i2s.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_irda.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_iwdg.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_mmc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_msp_template.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_nand.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_nor.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_pccard.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_pcd.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_pcd_ex.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_pwr.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_rcc.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_rcc_ex.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_rtc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_rtc_ex.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_sd.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_smartcard.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_spi.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_spi_ex.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_sram.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_tim.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_timebase_rtc_alarm_template.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_timebase_tim_template.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_tim_ex.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_uart.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_usart.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_wwdg.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_adc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_crc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_dac.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_dma.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_exti.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_fsmc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_gpio.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_i2c.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_pwr.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_rcc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_rtc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_sdmmc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_spi.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_tim.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_usart.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_usb.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_utils.c

    ${CMSIS_BASE}/Device/ST/${STM32_FAMILY_LONG}xx/Source/Templates/gcc/startup_${STM32_FAMILY_Xl}.s

    ${CMAKE_CURRENT_SOURCE_DIR}/system_${STM32_FAMILY_LONG_lx}.c
)

ADD_LIBRARY(STM32_HAL_STM32F042x6 STATIC ${HAL_SRC})
target_include_directories(STM32_HAL_STM32F042x6 PUBLIC
    ${HAL_BASE}/Inc
    ${CMSIS_BASE}/Include
    ${CMSIS_BASE}/Device/ST/${STM32_FAMILY_LONG}xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_compile_definitions(STM32_HAL_STM32F042x6 PUBLIC
    -DHAL_TIM_MODULE_ENABLED
)

