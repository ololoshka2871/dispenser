#ifndef _ABSTRACT_STEPPER_DRIVER_
#define _ABSTRACT_STEPPER_DRIVER_

struct FreeRunningAccelStepper;

struct AbstractStepDriver {
  AbstractStepDriver(FreeRunningAccelStepper &stepper);

  AbstractStepDriver &setEnabled(bool enable);

protected:
  FreeRunningAccelStepper &stepper;
  bool enabled;
};

#endif /* _ABSTRACT_STEPPER_DRIVER_ */
