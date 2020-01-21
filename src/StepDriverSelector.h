#ifndef _STEP_DRIVER_SELECTOR_
#define _STEP_DRIVER_SELECTOR_

#include <cstdint>
#include <vector>

struct AbstractStepDriver;

struct StepDriverSelector {
  StepDriverSelector(std::initializer_list<AbstractStepDriver *> drivers);

  void SelectDriver(size_t number);
  void DisableAll();

private:
  void apply_selection();
  void doDisableAll();

  std::vector<AbstractStepDriver *> drivers;
  int32_t selected;
};

#endif /* _STEP_DRIVER_SELECTOR_ */
