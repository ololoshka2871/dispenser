#ifndef _MANUAL_DRIVER_H_
#define _MANUAL_DRIVER_H_

#include "AbstractStepDriver.h"
#include "FreeRunningAccelStepper.h"

struct ManualDriver : public AbstractStepDriver {

  static void begin(FreeRunningAccelStepper &stepper, float max_speed,
                    float acceleration);
  static ManualDriver &instance();

  ManualDriver(const ManualDriver &) = delete;

  AbstractStepDriver &setEnabled(bool enable) override;

  void move(FreeRunningAccelStepper::Direction dir);
  void stop();

private:
  ManualDriver(FreeRunningAccelStepper &stepper, float max_speed,
               float acceleration);

  float max_speed;
  float acceleration;
};

#endif /* _MANUAL_DRIVER_H_ */
