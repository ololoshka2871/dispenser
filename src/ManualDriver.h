#ifndef _MANUAL_DRIVER_H_
#define _MANUAL_DRIVER_H_

#include "AbstractStepDriver.h"
#include "FreeRunningAccelStepper.h"

struct ManualDriver : public AbstractStepDriver {
  ManualDriver(FreeRunningAccelStepper &stepper);
  ManualDriver(const ManualDriver &) = delete;

  void move(FreeRunningAccelStepper::Direction dir);
  void stop();
};

#endif /* _MANUAL_DRIVER_H_ */
