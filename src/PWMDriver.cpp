#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

#include "PWMDriver.h"

#define PWM_port GPIOA
#define PWM_pin GPIO_PIN_6
#define PWM_pin_AF GPIO_AF1_TIM3

#define PWM_active_lvl 1

#define PWM_timer TIM3
#define PWM_CH1 TIM_CHANNEL_1
#define PWM_CH2 TIM_CHANNEL_2
#define TIMx_IRQHandler TIM3_IRQHandler

#define PWM_pin_IRQn IRQ

static TIM_HandleTypeDef pwm_tim{
    PWM_timer,
    {
        F_CPU / 1000000,               // Prescaler
        TIM_COUNTERMODE_UP,            // CounterMode
        0,                             // Period
        TIM_CLOCKDIVISION_DIV1,        // ClockDivision
        TIM_AUTORELOAD_PRELOAD_DISABLE // AutoReloadPreload
    }};

// config for external interrupt
static GPIO_InitTypeDef pwm_pin_exti {
  PWM_pin,
#if PWM_active_lvl
      GPIO_MODE_IT_FALLING,
#else
      GPIO_MODE_IT_RISING,
#endif
      GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM
};

// config for capture
static GPIO_InitTypeDef pwm_pin_tim{PWM_pin, GPIO_MODE_AF_PP, GPIO_NOPULL,
                                    GPIO_SPEED_FREQ_LOW, PWM_pin_AF};

static PWMDriver *inst;

void TIMx_IRQHandler(void) { HAL_TIM_IRQHandler(&pwm_tim); }

void PWMDriver::begin(FreeRunningAccelStepper &stepper) {
  if (inst) {
    delete inst;
  }
  inst = new PWMDriver{stepper};
}

PWMDriver &PWMDriver::instance() { return *inst; }

PWMDriver::PWMDriver(FreeRunningAccelStepper &stepper)
    : AbstractStepDriver{stepper} {

  HAL_TIM_Base_Init(&pwm_tim);

  TIM_ClockConfigTypeDef sClockSourceConfig{TIM_CLOCKSOURCE_INTERNAL};

  HAL_TIM_ConfigClockSource(&pwm_tim, &sClockSourceConfig);

  HAL_TIM_IC_Init(&pwm_tim);

  TIM_MasterConfigTypeDef sMasterConfig{TIM_TRGO_RESET,
                                        TIM_MASTERSLAVEMODE_DISABLE};
  HAL_TIMEx_MasterConfigSynchronization(&pwm_tim, &sMasterConfig);

  TIM_IC_InitTypeDef sConfigIC{TIM_INPUTCHANNELPOLARITY_RISING,
                               TIM_ICSELECTION_DIRECTTI, TIM_ICPSC_DIV1, 0};
  HAL_TIM_IC_ConfigChannel(&pwm_tim, &sConfigIC, PWM_CH1);
  HAL_TIM_IC_ConfigChannel(&pwm_tim, &sConfigIC, PWM_CH2);
}
