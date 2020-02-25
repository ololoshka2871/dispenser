#ifndef _RETRACKTOR_H_
#define _RETRACKTOR_H_

#include <cstdint>

#include "FreeRunningAccelStepper.h"

struct Retracktor {
  Retracktor(FreeRunningAccelStepper &stepper, uint16_t retract_distance,
             float retract_speed,
             FreeRunningAccelStepper::Direction direction =
                 FreeRunningAccelStepper::DIRECTION_CW);

  void reset();
  void doStartSerqunce();
  void doStopSequence();
  void setWorkSpeed(float workingSpeed);

  void process();

  enum State { READY, PREPARING, WORKING, RETRAKTING, RETRAKTED };

private:
  FreeRunningAccelStepper &stepper;
  uint16_t retract_distance;
  float retract_speed;
  FreeRunningAccelStepper::Direction direction;
  float workingSpeed;
  State state;
};

#endif /* _RETRACKTOR_H_ */
