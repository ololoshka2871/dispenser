#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

#include "EXTI_manager.h"
#include "FreeRunningAccelStepper.h"

#include "PWMDriver.h"

#define PWM_port GPIOA
#define PWM_pin GPIO_PIN_6
#define PWM_pin_AF GPIO_AF1_TIM3

#define PWM_active_lvl 1

#define PWM_pin_IRQn EXTI4_15_IRQn

#define PWM_timer TIM3
#define PWM_timer_clocking __HAL_RCC_TIM3_CLK_ENABLE
#define PWM_CH1 TIM_CHANNEL_1
#define PWM_CH2 TIM_CHANNEL_2
#define PWM_CH1_CCF TIM_FLAG_CC1
#define PWM_CH2_CCF TIM_FLAG_CC2
#define TIMx_IRQn TIM3_IRQn
#define TIMx_IRQHandler TIM3_IRQHandler

static TIM_HandleTypeDef pwm_tim{
    PWM_timer,
    {
        F_CPU / 1000000,               // Prescaler
        TIM_COUNTERMODE_UP,            // CounterMode
        0,                             // Period
        TIM_CLOCKDIVISION_DIV1,        // ClockDivision
        TIM_AUTORELOAD_PRELOAD_DISABLE // AutoReloadPreload
    }};

// config for external interrupt start
static GPIO_InitTypeDef pwm_pin_exti_start {
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

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  // Сработало прерывание захвата 2 значит мы отработали полный цыкл
  // получаем захваченные значения, отправляем на обработку, стартуем новый цыкл

  HAL_TIM_Base_Stop_IT(&pwm_tim);

  auto cv1 = __HAL_TIM_GET_COMPARE(&pwm_tim, PWM_CH1);
  auto cv2 = __HAL_TIM_GET_COMPARE(&pwm_tim, PWM_CH2);

  __HAL_TIM_CLEAR_FLAG(&pwm_tim, PWM_CH1_CCF | PWM_CH2_CCF);

  auto &drv = PWMDriver::instance();
  drv.ready(cv1, cv2);
  drv.startCycle();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // Прерывание переполнения

  HAL_TIM_Base_Stop_IT(&pwm_tim);

  auto &drv = PWMDriver::instance();

  // Нужно узнать, произошел-ли захват 1,
  if (__HAL_TIM_GET_FLAG(&pwm_tim, PWM_CH1_CCF)) {
    // ехать не надо, только сброс в начальное сстояние
    drv.ready(0, 0);
  } else {
    // то считать ШИМ 100%, настроить прерывание на Falling, которое
    // переверет систему в готовность
    drv.ready(1, 1);
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
      cycle_ready{false}, max_speed{max_speed}, start_it_hendler{[this]() {
        this->exti_manager.UnRegisterCallback(PWM_port, PWM_pin);
        startCycle();
      }},
      stop_it_hendler{[this]() {
        this->exti_manager.UnRegisterCallback(PWM_port, PWM_pin);
        prepare();
      }} {

  PWM_timer_clocking();

  HAL_TIM_Base_Init(&pwm_tim);

  TIM_ClockConfigTypeDef sClockSourceConfig{TIM_CLOCKSOURCE_INTERNAL};

  HAL_TIM_ConfigClockSource(&pwm_tim, &sClockSourceConfig);

  HAL_TIM_IC_Init(&pwm_tim);

  TIM_MasterConfigTypeDef sMasterConfig{TIM_TRGO_RESET,
                                        TIM_MASTERSLAVEMODE_DISABLE};
  HAL_TIMEx_MasterConfigSynchronization(&pwm_tim, &sMasterConfig);

  TIM_IC_InitTypeDef sConfigIC {
#if PWM_active_lvl
    TIM_INPUTCHANNELPOLARITY_FALLING,
#else
    TIM_INPUTCHANNELPOLARITY_RISING,
#endif
        TIM_ICSELECTION_DIRECTTI, TIM_ICPSC_DIV1, 0
  };
  HAL_TIM_IC_ConfigChannel(&pwm_tim, &sConfigIC, PWM_CH1);

  sConfigIC.ICPolarity =
#if PWM_active_lvl
      TIM_INPUTCHANNELPOLARITY_RISING
#else
      TIM_INPUTCHANNELPOLARITY_FALLING
#endif
      ;
  HAL_TIM_IC_ConfigChannel(&pwm_tim, &sConfigIC, PWM_CH2);

  // interrupts
  NVIC_SetPriority(TIMx_IRQn, 5);
}

void PWMDriver::prepare() {
  regisetr_exti(&start_it_hendler, pwm_pin_exti_start);
}

void PWMDriver::deinitPin_counter() { HAL_GPIO_DeInit(PWM_port, PWM_pin); }

void PWMDriver::disableAll() {
  unregister_exti();
  deinitPin_counter();
  NVIC_DisableIRQ(TIMx_IRQn);

  stepper.stopHard();
}

void PWMDriver::regisetr_exti(const std::function<void()> *f,
                              GPIO_InitTypeDef &cfg) {
  exti_manager.RegisterCallback(f, PWM_port, cfg);
}

void PWMDriver::unregister_exti() {
  exti_manager.UnRegisterCallback(PWM_port, PWM_pin);
}

void PWMDriver::startCycle() {
  __HAL_TIM_SET_COUNTER(&pwm_tim, 0);     // reset counter
  HAL_TIM_IC_Start(&pwm_tim, PWM_CH1);    // enable chanel 1 caplure, no IT
  HAL_TIM_IC_Start_IT(&pwm_tim, PWM_CH2); // enable chanel 2 capture, w/ IT
  HAL_TIM_Base_Start_IT(&pwm_tim);        // enable overflow w/ IT
}

void PWMDriver::ready(uint32_t duration, uint32_t period) {
  if (duration == period) {
    deinitPin_counter();
    NVIC_DisableIRQ(TIMx_IRQn);
    if (duration) {
      // 100% шим
      GPIO_InitTypeDef pwm_pin_exti_stop{pwm_pin_exti_start};
      pwm_pin_exti_stop.Mode =
#if PWM_active_lvl
          GPIO_MODE_IT_RISING
#else
          GPIO_MODE_IT_FALLING
#endif
          ;

      regisetr_exti(&stop_it_hendler, pwm_pin_exti_stop);
    } else {
      // reset
      start();
    }
  }
  this->duration = duration;
  this->period = period;
  cycle_ready = true;
}

void PWMDriver::start() {
  unregister_exti();
  HAL_GPIO_Init(PWM_port, &pwm_pin_tim);
  startCycle();
}

AbstractStepDriver &PWMDriver::setEnabled(bool enable) {
  if (enable) {
    prepare();
  } else {
    disableAll();
  }
  return AbstractStepDriver::setEnabled(enable);
}

void PWMDriver::process() {
  if (cycle_ready && enabled) {
    // do calculations, update stepper settings
    if (period) {
      float dest_speed = max_speed * duration / period;

      stepper.setMaxSpeed(dest_speed);
      stepper.moveFree(FreeRunningAccelStepper::DIRECTION_CW);
    } else {
      stepper.stop();
    }
  }
}

void PWMDriver::trigger() {
  auto &_this = instance();
  _this.deinitPin_counter();
  _this.start();
}
