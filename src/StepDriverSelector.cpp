#include <algorithm>
#include <cassert>

#include "AbstractStepDriver.h"

#include "StepDriverSelector.h"

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
  /*
std::for_each(drivers.begin(), drivers.end(),
              [](auto d) { d->setEnabled(false); });
              */
  for (auto it = drivers.begin(); it != drivers.end(); ++it) {
    (*it)->setEnabled(false);
  }
}
