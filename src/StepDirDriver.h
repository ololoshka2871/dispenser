#ifndef _STEPDIR_DRIVER_H_
#define _STEPDIR_DRIVER_H_

#include "AbstractStepDriver.h"

struct StepDirDriver : AbstractStepDriver {
  StepDirDriver(FreeRunningAccelStepper &stepper);
  StepDirDriver(const StepDirDriver &) = delete;
};

#endif /* _STEPDIR_DRIVER_H_ */
