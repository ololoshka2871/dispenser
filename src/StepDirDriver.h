#ifndef _STEPDIR_DRIVER_H_
#define _STEPDIR_DRIVER_H_

#include <functional>

#include "AbstractStepDriver.h"

struct EXTI_manager_base;

struct StepDirDriver : AbstractStepDriver {
  StepDirDriver(const StepDirDriver &) = delete;
  ~StepDirDriver();

  AbstractStepDriver &setEnabled(bool enable) override;

  static void begin(FreeRunningAccelStepper &stepper,
                    EXTI_manager_base &exti_manager, bool invert_dir = false);
  static StepDirDriver &instance();

  void doStep();

private:
  StepDirDriver(FreeRunningAccelStepper &stepper,
                EXTI_manager_base &exti_manager, bool invert_dir);

  EXTI_manager_base &exti_manager;
  const std::function<void()> step_ex;
  EXTI_manager_base &Exti_manager;
  bool invert_dir;
};

#endif /* _STEPDIR_DRIVER_H_ */
