#include <AccelStepper.h>

#include "ManualDriver.h"

ManualDriver::ManualDriver(FreeRunningAccelStepper &_stepper)
    : AbstractStepDriver{_stepper} {}

void ManualDriver::move(FreeRunningAccelStepper::Direction dir) {
  if (enabled) {
    stepper.moveFree(dir);
  }
}

void ManualDriver::stop() {
  if (enabled) {
    stepper.stop();
  }
}
