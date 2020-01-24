#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

#include <DigitalOut.h>

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
  // Сработало прерывание захвата 2

  auto cv1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
  auto cv2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
  if (cv1 && cv2) {
    PWMDriver::instance().ready(cv1, cv2);
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // Прерывание переполнения

  auto &drv = PWMDriver::instance();

  auto state = __HAL_TIM_GET_FLAG(&pwm_tim, TIM_FLAG_CC1) |
               (__HAL_TIM_GET_FLAG(&pwm_tim, TIM_FLAG_CC2) << 1);

  // Нужно узнать, произошел-ли захват 1,
  if (__HAL_TIM_GET_FLAG(&pwm_tim, TIM_FLAG_CC1)) {
    __HAL_TIM_CLEAR_IT(&pwm_tim, TIM_IT_CC1);
    // ехать не надо, единичный импульс
    drv.ready(state, 0);
  } else {
    // 2 варианта:
    // если уровень сейчас активный, то считать ШИМ 100%
    // если уровень сейчас не активный - ни чего нет на входе - стоп
    if (!__HAL_TIM_GET_FLAG(&pwm_tim, TIM_FLAG_CC2)) {
      // ехать не надо
      drv.ready(0, 0);
    } else {
      // макс скорость
      __HAL_TIM_CLEAR_IT(&pwm_tim, TIM_IT_CC2);
      drv.ready(1, 1);
    }
  }
}

void PWMDriver::begin(FreeRunningAccelStepper &stepper,
                      EXTI_manager_base &exti_manager, float max_speed) {
  if (inst) {
    delete inst;
  }
  inst = new PWMDriver{stepper, exti_manager, max_speed};
}

PWMDriver &PWMDriver::instance() { return *inst; }

PWMDriver::PWMDriver(FreeRunningAccelStepper &stepper,
                     EXTI_manager_base &exti_manager, float max_speed)
    : AbstractStepDriver{stepper}, exti_manager{exti_manager},
      cycle_ready{false}, max_speed{max_speed} {
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
                                      TIM_TRIGGERPOLARITY_NONINVERTED,
                                      TIM_TRIGGERPRESCALER_DIV1, 0};
  assert(HAL_TIM_SlaveConfigSynchronization(&pwm_tim, &sSlaveConfig) == HAL_OK);

  /**** Configure the master mode ****/
  TIM_MasterConfigTypeDef sMasterConfig{TIM_TRGO_RESET,
                                        TIM_MASTERSLAVEMODE_DISABLE};
  assert(HAL_TIMEx_MasterConfigSynchronization(&pwm_tim, &sMasterConfig) ==
         HAL_OK);

  /****  Configure the Input Capture ****/
  TIM_IC_InitTypeDef sConfigIC {
#if PWM_active_lvl
    TIM_INPUTCHANNELPOLARITY_FALLING,
#else
    TIM_INPUTCHANNELPOLARITY_RISING,
#endif
        TIM_ICSELECTION_DIRECTTI, TIM_ICPSC_DIV1, 0
  };
  assert(HAL_TIM_IC_ConfigChannel(&pwm_tim, &sConfigIC, TIM_CHANNEL_1) ==
         HAL_OK);

  sConfigIC.ICPolarity =
#if PWM_active_lvl
      TIM_INPUTCHANNELPOLARITY_RISING
#else
      TIM_INPUTCHANNELPOLARITY_FALLING
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

void PWMDriver::start() {
  this->duration = 0;
  this->period = 0;

  __HAL_TIM_CLEAR_IT(&pwm_tim, TIM_IT_UPDATE | TIM_IT_CC1 | TIM_IT_CC2);

  NVIC_ClearPendingIRQ(PWM_timer_IRQn);
  NVIC_EnableIRQ(PWM_timer_IRQn);

  __HAL_TIM_ENABLE(&pwm_tim);
}

AbstractStepDriver &PWMDriver::setEnabled(bool enable) {
  if (enable) {
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
    //*test_out = !(*test_out);
    if (enabled) {
      if (period) {
        float dest_speed = max_speed * duration / period;

        stepper.setMaxSpeed(dest_speed);
        // stepper.moveFree(FreeRunningAccelStepper::DIRECTION_CW);
      } else {
        // stepper.stop();
      }
    }
  }
}
