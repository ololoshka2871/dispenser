#include <algorithm>

#include "AbstractStepDriver.h"

#include "StepDriverSelector.h"

#define assert(x) __asm__("BKPT")

StepDriverSelector::StepDriverSelector(
    std::initializer_list<AbstractStepDriver *> drivers)
    : drivers{drivers}, selected(-1) {
  doDisableAll();
}

void StepDriverSelector::SelectDriver(size_t number) {
  assert(number < drivers.size());
  if (number != selected) {
    selected = number;
    apply_selection();
  }
}

void StepDriverSelector::DisableAll() {
  selected = -1;
  doDisableAll();
}

void StepDriverSelector::apply_selection() {
  if (drivers.size() == 0) {
    return;
  }

  assert(selected < drivers.size());

  doDisableAll();
  if (selected >= 0) {
    drivers[selected]->setEnabled(true);
  }
}

void StepDriverSelector::doDisableAll() {
  std::for_each(drivers.begin(), drivers.end(),
                [](auto d) { d->setEnabled(false); });
}
