#ifndef _FREE_RUNNING_ACCELSTEPPER_H_
#define _FREE_RUNNING_ACCELSTEPPER_H_

#include <AccelStepper.h>

struct FreeRunningAccelStepper : public AccelStepper {
  FreeRunningAccelStepper(uint8_t interface, DigitalOut &&pin1 = DigitalOut{},
                          DigitalOut &&pin2 = DigitalOut{},
                          DigitalOut &&pin3 = DigitalOut{},
                          DigitalOut &&pin4 = DigitalOut{}, bool enable = true);

  void moveFree(Direction dir);
  void stop();
  void stopHard();
  void run();

  float getAcceleration() const;

private:
  long stepsToStop;
  bool free_run;
};

#endif /* _FREE_RUNNING_ACCELSTEPPER_H_ */
