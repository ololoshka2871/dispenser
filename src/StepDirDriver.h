#ifndef _STEPDIR_DRIVER_H_
#define _STEPDIR_DRIVER_H_

#include "AbstractStepDriver.h"

struct StepDirDriver : AbstractStepDriver {
  StepDirDriver(const StepDirDriver &) = delete;
  ~StepDirDriver();

  AbstractStepDriver &setEnabled(bool enable) override;

  static void begin(FreeRunningAccelStepper &stepper, bool invert_dir = false);
  static StepDirDriver &instance();

  void doStep();

private:
  StepDirDriver(FreeRunningAccelStepper &stepper, bool invert_dir);

  bool invert_dir;
};

#endif /* _STEPDIR_DRIVER_H_ */
