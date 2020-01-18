/*!
 * \file
 * \brief Initialization of base MCU peripherals
 */

#include "stm32f0xx_hal.h"

/*!
 * \ingroup BootloaderHW
 *  @{
 */

// Need c++14! (not working as planned!)

/// Class-container of multiplier and divider for PLL
struct sDiv_Mul {
  /// Coded to MCU-specific values divider and multiplier
  uint32_t mul, div;

  /// Empty method to raise compile-time error then divider or multiplier
  /// incorrect
  void error(bool x) {}

  /*! Constructor. Set and check coded to MCU-specific values divider and
   * multiplier
   *
   * Raise compile-time error if values out of range
   */
  constexpr sDiv_Mul(int32_t m = -1, int32_t d = -1) : mul(m), div(d) {
    if ((m < RCC_PLL_MUL2) || (d < RCC_PREDIV_DIV1) || (m > RCC_PLL_MUL16) ||
        (d > RCC_PREDIV_DIV16))
      error(false);
  }
};

/*!
 * \brief Compile-time calculation of PLL divider and
 * multiplier
 *
 * Calculates proper value of PLL divider and multiplier to achieve CPU clock
 * from HSE value
 * \param div_mul value of Target_cpu_clock/HSE_value
 * \return struct sDiv_Mul, what contains codded PLL divider and
 * multiplier or compile-time error
 */
constexpr sDiv_Mul calc_pll_div_mul(const uint32_t div_mul) {
  const uint32_t muls[] = {RCC_PLL_MUL2,  RCC_PLL_MUL3,  RCC_PLL_MUL4,
                           RCC_PLL_MUL5,  RCC_PLL_MUL6,  RCC_PLL_MUL7,
                           RCC_PLL_MUL8,  RCC_PLL_MUL9,  RCC_PLL_MUL10,
                           RCC_PLL_MUL11, RCC_PLL_MUL12, RCC_PLL_MUL13,
                           RCC_PLL_MUL14, RCC_PLL_MUL15, RCC_PLL_MUL16};
  const uint32_t divs[] = {
      RCC_PREDIV_DIV1,  RCC_PREDIV_DIV2,  RCC_PREDIV_DIV3,  RCC_PREDIV_DIV4,
      RCC_PREDIV_DIV5,  RCC_PREDIV_DIV6,  RCC_PREDIV_DIV7,  RCC_PREDIV_DIV8,
      RCC_PREDIV_DIV9,  RCC_PREDIV_DIV10, RCC_PREDIV_DIV11, RCC_PREDIV_DIV12,
      RCC_PREDIV_DIV13, RCC_PREDIV_DIV14, RCC_PREDIV_DIV15, RCC_PREDIV_DIV16};
  for (uint32_t mul = 2; mul < sizeof(muls) / sizeof(uint32_t) + 2; ++mul) {
    for (uint32_t div = 1; div < sizeof(divs) / sizeof(uint32_t) + 1; ++div) {
      if (mul / div == div_mul) {
        return sDiv_Mul(muls[mul - 2], divs[div - 1]);
      }
    }
  }
  return sDiv_Mul();
}

/*!
 * \brief Initialize CPU clicking by setup PLL correct values
 *
 * Uses MACROs HSE_VALUE and F_CPU to calculate and set PLL register values in
 * compile-time.
 * MCU family specific realization
 */
void InitOSC() {
  constexpr auto div_mul = calc_pll_div_mul(F_CPU / HSE_VALUE);
  constexpr RCC_OscInitTypeDef RCC_OscInitStruct = {
      // HSE_VALUE -> PREDIV -> PLLMUL => F_CPU
      // PREDIV / PLLMUL = F_CPU / HSE_VALUE

      .OscillatorType = RCC_OSCILLATORTYPE_HSE,
      .HSEState = RCC_HSE_ON,
      .LSEState = RCC_LSE_OFF,
      .HSIState = RCC_HSI_OFF,
      .HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
      .HSI14State = RCC_HSI14_OFF,
      .HSI14CalibrationValue = RCC_HSI14CALIBRATION_DEFAULT,
      .LSIState = RCC_LSI_OFF,
      .HSI48State = RCC_HSI48_OFF,
      .PLL = {
          .PLLState = RCC_PLL_ON,
          .PLLSource = RCC_PLLSOURCE_HSE,
          .PLLMUL = div_mul.mul,
          .PREDIV = div_mul.div,
      }};

  HAL_RCC_OscConfig((RCC_OscInitTypeDef *)&RCC_OscInitStruct);
}

/*!
 * \brief Setup MCU peripheral buses clocks
 *
 * MCU-family specific realization
 */
void Configure_AHB_Clocks() {
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;

  RCC_ClkInitStruct.APB1CLKDivider =
      F_CPU > 24000000U ? RCC_HCLK_DIV2 : RCC_HCLK_DIV1;
  auto latency = F_CPU < 24000000U ? FLASH_LATENCY_0 : FLASH_LATENCY_1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, latency);
}

/*!
 * \brief Configures MCU base clocking and setup system timer
 */
void SystemClock_Config(void) {
  InitOSC();
  Configure_AHB_Clocks();

  // Set up SysTTick to 1 ms
  // TODO: Do we really need this? SysTick is initialized multiple times in HAL
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  // SysTick_IRQn interrupt configuration - setting SysTick as lower priority to
  // satisfy FreeRTOS requirements
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/*!
 * \brief Main initialization function
 */
void InitBoard() {
  // Initialize board and HAL
  HAL_Init();

  // defined in HAL_Init()
  // HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  SystemClock_Config();
}

/*! @} */
