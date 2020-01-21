#ifndef _PWM_DRIVER_H_
#define _PWM_DRIVER_H_

#include "AbstractStepDriver.h"

struct PWMDriver : AbstractStepDriver {
  static void begin(FreeRunningAccelStepper &stepper);
  static PWMDriver &instance();

  PWMDriver(const PWMDriver &) = delete;

private:
  PWMDriver(FreeRunningAccelStepper &stepper);
};

#endif /* _PWM_DRIVER_H_ */
