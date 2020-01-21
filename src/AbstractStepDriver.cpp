#include "AbstractStepDriver.h"

AbstractStepDriver::AbstractStepDriver(FreeRunningAccelStepper &stepper)
    : stepper(stepper), enabled{false} {}

AbstractStepDriver &AbstractStepDriver::setEnabled(bool enable) {
  this->enabled = enable;
  return *this;
}
