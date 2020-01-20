#ifndef _UI_H_
#define _UI_H_

#include "Button.h"

struct UI {
  UI();

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

  void apply_state(State newstate);

  static void setupLed(State nestate);

  Button<ButtonID> buttons[ButtonID::COUNT];
  State state;
};

#endif /* _UI_H_ */
