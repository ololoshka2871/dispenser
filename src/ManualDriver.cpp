#include <AccelStepper.h>

#include "ManualDriver.h"

static ManualDriver *inst;

void ManualDriver::begin(FreeRunningAccelStepper &stepper, float max_speed,
                         float acceleration) {
  if (inst) {
    delete inst;
  }
  inst = new ManualDriver{stepper, max_speed, acceleration};
}

ManualDriver &ManualDriver::instance() { return *inst; }

ManualDriver::ManualDriver(FreeRunningAccelStepper &_stepper, float max_speed,
                           float acceleration)
    : AbstractStepDriver{_stepper}, max_speed{max_speed}, acceleration{
                                                              acceleration} {}

AbstractStepDriver &ManualDriver::setEnabled(bool enable) {
  if (enable) {
    stepper.setMaxSpeed(max_speed);
    stepper.setAcceleration(acceleration);
  }
  return AbstractStepDriver::setEnabled(enable);
}

void ManualDriver::move(FreeRunningAccelStepper::Direction dir) {
  if (enabled) {
    stepper.enableOutputs();
    stepper.moveFree(dir);
  }
}

void ManualDriver::stop() {
  if (enabled) {
    stepper.disableOutputs();
    stepper.stopHard();
  }
}
