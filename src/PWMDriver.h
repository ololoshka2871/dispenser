#ifndef _PWM_DRIVER_H_
#define _PWM_DRIVER_H_

#include "AbstractStepDriver.h"

struct PWMDriver : AbstractStepDriver {
  PWMDriver(FreeRunningAccelStepper &stepper);
  PWMDriver(const PWMDriver &) = delete;
};

#endif /* _PWM_DRIVER_H_ */
