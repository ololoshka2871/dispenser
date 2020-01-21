#include <AccelStepper.h>

#include "ManualDriver.h"

static ManualDriver *inst;

void ManualDriver::begin(FreeRunningAccelStepper &stepper, float acceleration) {
  if (inst) {
    delete inst;
  }
  inst = new ManualDriver{stepper, acceleration};
}

ManualDriver &ManualDriver::instance() { return *inst; }

ManualDriver::ManualDriver(FreeRunningAccelStepper &_stepper,
                           float acceleration)
    : AbstractStepDriver{_stepper}, acceleration(acceleration),
      _saveAcceleration(acceleration) {}

AbstractStepDriver &ManualDriver::setEnabled(bool enable) {
  if (enable) {
    _saveAcceleration = stepper.getAcceleration();
    stepper.setAcceleration(acceleration);
  } else {
    stepper.setAcceleration(_saveAcceleration);
  }
  return AbstractStepDriver::setEnabled(enable);
}

void ManualDriver::move(FreeRunningAccelStepper::Direction dir) {
  if (enabled) {
    stepper.moveFree(dir);
    // stepper.doStep(FreeRunningAccelStepper::DIRECTION_CW);
  }
}

void ManualDriver::stop() {
  if (enabled) {
    stepper.stopHard();
    // stepper.doStep(FreeRunningAccelStepper::DIRECTION_CCW);
  }
}
