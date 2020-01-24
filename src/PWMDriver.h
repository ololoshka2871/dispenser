#ifndef _PWM_DRIVER_H_
#define _PWM_DRIVER_H_

#include <cstdint>
#include <functional>

#include "AbstractStepDriver.h"

struct EXTI_manager_base;

#ifndef __STM32F0xx_HAL_GPIO_H
struct GPIO_InitTypeDef;
#endif

struct PWMDriver : AbstractStepDriver {
  static void begin(FreeRunningAccelStepper &stepper,
                    EXTI_manager_base &exti_manager, float max_speed);
  static PWMDriver &instance();

  PWMDriver(const PWMDriver &) = delete;

  AbstractStepDriver &setEnabled(bool enable) override;

  void process();

  // do not call this, for internal use only
  void ready(uint32_t duration, uint32_t period);

private:
  EXTI_manager_base &exti_manager;
  volatile uint32_t duration;
  volatile uint32_t period;
  volatile bool cycle_ready;

  float max_speed;

  PWMDriver(FreeRunningAccelStepper &stepper, EXTI_manager_base &exti_manager,
            float max_speed);

  void stop();

  void start();
};

#endif /* _PWM_DRIVER_H_ */
