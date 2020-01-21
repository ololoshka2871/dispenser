#include "FreeRunningAccelStepper.h"

#include "AbstractStepDriver.h"

AbstractStepDriver::AbstractStepDriver(FreeRunningAccelStepper &stepper)
    : stepper(stepper), enabled{false} {}

AbstractStepDriver &AbstractStepDriver::setEnabled(bool enable) {
  this->enabled = enable;
  if (enable) {
    stepper.enableOutputs();
  } else {
    stepper.disableOutputs();
  }
  return *this;
}
