#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_tim_ex.h>

#include "LedController.h"

#include "FreeRunningAccelStepper.h"

#include "PWMDriver.h"

#define PWM_port GPIOA
#define PWM_pin GPIO_PIN_6
#define PWM_pin_AF GPIO_AF1_TIM3

#define PWM_active_lvl 1

#define PWM_timer TIM3
#define PWM_timer_clocking __HAL_RCC_TIM3_CLK_ENABLE

#define PWM_timer_IRQn TIM3_IRQn
#define PWM_timer_IRQHandler TIM3_IRQHandler

#define MIN_SPEED 100

#define assert(x)                                                              \
  if (!(x)) {                                                                  \
    __asm__("BKPT");                                                           \
  }

static TIM_HandleTypeDef pwm_tim{
    PWM_timer,
    {
        F_CPU / 1000000,               // Prescaler
        TIM_COUNTERMODE_UP,            // CounterMode
        0xffff,                        // Period
        TIM_CLOCKDIVISION_DIV1,        // ClockDivision
        TIM_AUTORELOAD_PRELOAD_DISABLE // AutoReloadPreload
    }};

static PWMDriver *inst;

extern "C" void PWM_timer_IRQHandler(void) { HAL_TIM_IRQHandler(&pwm_tim); }

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  // Сработало прерывание захвата
  __HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);

  auto &drv = PWMDriver::instance();

  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
    // raising

    if (drv.getState() == PWMDriver::FALL_CAPTURED) {
      auto t = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
      auto duration = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
      drv.ready(t, duration);
    }

    drv.setState(PWMDriver::START);
  } else {
    // falling
    drv.setState(PWMDriver::FALL_CAPTURED);
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // Прерывание переполнения

  auto &drv = PWMDriver::instance();

  switch (drv.getState()) {
  case PWMDriver::RESET: // таймаут, стоп
    drv.ready(0, 0);
    break;
  case PWMDriver::FALL_CAPTURED: // одиночный импульс
    drv.setState(PWMDriver::RESET);
    drv.ready(0, 0);
    break;
  case PWMDriver::START: // таймаут после старта -> PWM 100%
    drv.ready(1, 1);
    drv.setState(PWMDriver::HOLD_100);
    break;
  case PWMDriver::HOLD_100: // ждум конца PWM 100%
    break;
  }
}

void PWMDriver::begin(FreeRunningAccelStepper &stepper,
                      EXTI_manager_base &exti_manager, float max_speed,
                      float acceleration) {
  if (inst) {
    delete inst;
  }
  inst = new PWMDriver{stepper, exti_manager, max_speed, acceleration};
}

PWMDriver &PWMDriver::instance() { return *inst; }

PWMDriver::PWMDriver(FreeRunningAccelStepper &stepper,
                     EXTI_manager_base &exti_manager, float max_speed,
                     float acceleration)
    : AbstractStepDriver{stepper}, exti_manager{exti_manager},
      cycle_ready{false}, max_speed{max_speed}, acceleration{acceleration} {
  PWM_timer_clocking();

  // input pin
  GPIO_InitTypeDef pwm_pin_tim{PWM_pin, GPIO_MODE_AF_PP, GPIO_NOPULL,
                               GPIO_SPEED_FREQ_LOW, PWM_pin_AF};
  HAL_GPIO_Init(PWM_port, &pwm_pin_tim);

  /**** Input compere init ****/
  assert(HAL_TIM_IC_Init(&pwm_tim) == HAL_OK);

  /**** Clock source ****/
  TIM_ClockConfigTypeDef sClockSourceConfig{TIM_CLOCKSOURCE_INTERNAL};
  assert(HAL_TIM_ConfigClockSource(&pwm_tim, &sClockSourceConfig) == HAL_OK);

  /**** Configure the slave mode ****/
  TIM_SlaveConfigTypeDef sSlaveConfig{TIM_SLAVEMODE_RESET, TIM_TS_TI1FP1,
                                      TIM_TRIGGERPOLARITY_FALLING,
                                      TIM_TRIGGERPRESCALER_DIV1, 0};
  assert(HAL_TIM_SlaveConfigSynchro(&pwm_tim, &sSlaveConfig) == HAL_OK);

  /**** Configure the master mode ****/
  TIM_MasterConfigTypeDef sMasterConfig{TIM_TRGO_RESET,
                                        TIM_MASTERSLAVEMODE_DISABLE};
  assert(HAL_TIMEx_MasterConfigSynchronization(&pwm_tim, &sMasterConfig) ==
         HAL_OK);

  /****  Configure the Input Capture ****/
  TIM_IC_InitTypeDef sConfigIC {
#if PWM_active_lvl
    TIM_INPUTCHANNELPOLARITY_RISING,
#else
    TIM_INPUTCHANNELPOLARITY_FALLING,
#endif
        TIM_ICSELECTION_DIRECTTI, TIM_ICPSC_DIV1, 0
  };
  assert(HAL_TIM_IC_ConfigChannel(&pwm_tim, &sConfigIC, TIM_CHANNEL_1) ==
         HAL_OK);

  sConfigIC.ICPolarity =
#if PWM_active_lvl
      TIM_INPUTCHANNELPOLARITY_FALLING
#else
      TIM_INPUTCHANNELPOLARITY_RISING
#endif
      ;

  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  assert(HAL_TIM_IC_ConfigChannel(&pwm_tim, &sConfigIC, TIM_CHANNEL_2) ==
         HAL_OK);

  // enable chanel 1 capture, w/o IT
  __HAL_TIM_ENABLE_IT(&pwm_tim, TIM_IT_CC1);
  TIM_CCxChannelCmd(pwm_tim.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);

  // enable chanel 2 capture, w/ IT
  __HAL_TIM_ENABLE_IT(&pwm_tim, TIM_IT_CC2);
  TIM_CCxChannelCmd(pwm_tim.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);

  // enable overflow w/ IT
  __HAL_TIM_ENABLE_IT(&pwm_tim, TIM_IT_UPDATE);

  // interrupts
  NVIC_SetPriority(PWM_timer_IRQn, 5);
}

void PWMDriver::stop() {
  __HAL_TIM_DISABLE(&pwm_tim);
  NVIC_DisableIRQ(PWM_timer_IRQn);

  stepper.stopHard();
}

void PWMDriver::ready(uint32_t duration, uint32_t period) {
  if (period != this->period || duration != this->duration) {
    this->duration = duration;
    this->period = period;
    cycle_ready = true;
  }
}

void PWMDriver::setState(PWMDriver::State s) { state = s; }

PWMDriver::State PWMDriver::getState() const { return state; }

void PWMDriver::start() {
  state = RESET;
  this->duration = 0;
  this->period = 0;

  __HAL_TIM_CLEAR_IT(&pwm_tim, TIM_IT_UPDATE | TIM_IT_CC1 | TIM_IT_CC2);

  NVIC_ClearPendingIRQ(PWM_timer_IRQn);
  NVIC_EnableIRQ(PWM_timer_IRQn);

  __HAL_TIM_ENABLE(&pwm_tim);
}

AbstractStepDriver &PWMDriver::setEnabled(bool enable) {
  if (enable) {
    stepper.setAcceleration(acceleration);
    start();
  } else {
    stop();
  }
  return AbstractStepDriver::setEnabled(enable);
}

void PWMDriver::process() {
  if (cycle_ready) {
    // do calculations, update stepper settings
    cycle_ready = false;
    if (enabled) {
      if (period) {
        LedController::setBlink(LedController::BLINK_FAST);
        float dest_speed = max_speed * duration / period;

        stepper.setMaxSpeed(dest_speed < MIN_SPEED ? MIN_SPEED : dest_speed);
        stepper.moveFree(FreeRunningAccelStepper::DIRECTION_CW);
      } else {
        stepper.stopHard();
        LedController::setBlink(LedController::BLINK_NO);
      }
    }
  }
}
