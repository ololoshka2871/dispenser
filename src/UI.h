#ifndef _UI_H_
#define _UI_H_

#include "Button.h"

struct StepDriverSelector;
struct ManualDriver;

struct UI {
  UI(StepDriverSelector &selector, ManualDriver &manualDriver);

  void process();

private:
  enum ButtonID { UP = 0, SEL, DOWN, COUNT };

  enum State {
    MENU_MANUAL = 0,
    MENU_PWM = 1,
    MENU_STEPDIR = 2,

    MODE_COUNT = 3,

    MANUAL,
    PWM,
    STEP_DIR
  };

  enum mode {
    _MANUAL = 0,
    _PWM = 1,
    _STEPDIR = 2,
  };

  void apply_state(State newstate);

  static void setupLed(State nestate);
  static mode state2Mode(State state);

  Button<ButtonID> buttons[ButtonID::COUNT];
  State state;

  StepDriverSelector &selector;
  ManualDriver &manualDriver;
};

#endif /* _UI_H_ */
