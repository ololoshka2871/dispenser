#include <stm32f0xx_hal.h>

#include "UsTimer.h"

static UsTimer *inst = nullptr;

static uint16_t extender = 0;

static TIM_HandleTypeDef tim14{
    TIM14,
    {
        F_CPU / 1000000,               // Prescaler
        TIM_COUNTERMODE_UP,            // CounterMode
        0xffff,                        // Period
        TIM_CLOCKDIVISION_DIV1,        // ClockDivision
        0,                             // RepetitionCounter
        TIM_AUTORELOAD_PRELOAD_DISABLE // AutoReloadPreload
    }};

extern "C" void TIM14_IRQHandler() {
  __HAL_TIM_CLEAR_IT(&tim14, TIM_IT_UPDATE);
  ++extender;
}

UsTimer &UsTimer::Instance() {
  if (!inst) {
    inst = new UsTimer();
  }
  return *inst;
}

uint32_t UsTimer::read_us() {
  return (((uint32_t)extender) << 16) | __HAL_TIM_GET_COUNTER(&tim14);
}

UsTimer::UsTimer() {
  // init TIM14
  __HAL_RCC_TIM14_CLK_ENABLE();

  HAL_TIM_Base_Init(&tim14);
  HAL_TIM_Base_Start_IT(&tim14);

  NVIC_SetPriority(TIM14_IRQn, 8);
  NVIC_EnableIRQ(TIM14_IRQn);
}
