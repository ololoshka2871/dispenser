#include "Retracktor.h"

Retracktor::Retracktor(FreeRunningAccelStepper &stepper,
                       uint16_t retract_distance, float retract_speed,
                       AccelStepper::Direction direction)
    : stepper{stepper}, retract_distance{retract_distance},
      retract_speed{retract_speed}, direction{direction},
      workingSpeed(0), state{READY} {}

void Retracktor::reset() {
  if (state == RETRAKTED)
    state = READY;
}

void Retracktor::doStartSerqunce() {
  switch (state) {
  case READY:
    stepper.enableOutputs();
    stepper.setMaxSpeed(workingSpeed);
    stepper.moveFree(direction);
    state = WORKING;
    break;
  case RETRAKTED:
    stepper.enableOutputs();
    stepper.setSpeed(retract_speed);
    stepper.move(
        FreeRunningAccelStepper::destApplyDir(direction, retract_distance));
    state = PREPARING;
    break;
  default:
    break;
  }
}

void Retracktor::doStopSequence() {
  switch (state) {
  case PREPARING:
    // stop and return to retrakt point
    {
      auto to_go = retract_distance - labs(stepper.distanceToGo()); // forward
      stepper.move(FreeRunningAccelStepper::destApplyDir(direction, -to_go));
      state = RETRAKTING;
    }
    break;
  case WORKING:
    // stop and retrakt
    stepper.stopHard();
    stepper.setMaxSpeed(retract_speed);
    stepper.move(
        FreeRunningAccelStepper::destApplyDir(direction, -retract_distance));
    state = RETRAKTING;
    break;
  default:
    break;
  }
}

void Retracktor::setWorkSpeed(float workingSpeed) {
  this->workingSpeed = workingSpeed;
  if (state == WORKING) {
    stepper.setMaxSpeed(workingSpeed);
  }
}

void Retracktor::process() {
  switch (state) {
  case PREPARING:
    if (!stepper.isRunning()) {
      stepper.setMaxSpeed(workingSpeed);
      stepper.moveFree(direction);
      state = WORKING;
    }
    break;
  case RETRAKTING:
    if (!stepper.isRunning()) {
      stepper.disableOutputs();
      state = RETRAKTED;
    }
    break;
  default:
    break;
  }
}
