#ifndef _MANUAL_DRIVER_H_
#define _MANUAL_DRIVER_H_

#include "AbstractStepDriver.h"
#include "FreeRunningAccelStepper.h"

struct ManualDriver : public AbstractStepDriver {
  ManualDriver(FreeRunningAccelStepper &stepper, float acceleration);
  ManualDriver(const ManualDriver &) = delete;

  AbstractStepDriver &setEnabled(bool enable) override;

  void move(FreeRunningAccelStepper::Direction dir);
  void stop();

private:
  float acceleration;
  float _saveAcceleration;
};

#endif /* _MANUAL_DRIVER_H_ */
