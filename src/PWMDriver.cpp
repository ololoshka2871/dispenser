#include "PWMDriver.h"

PWMDriver::PWMDriver(FreeRunningAccelStepper &stepper)
    : AbstractStepDriver{stepper} {}
