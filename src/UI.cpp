#include <cassert>
#include <map>

#include "LedController.h"

#include "UI.h"

UI::UI()
    : buttons{{ButtonID::UP, GPIOB, GPIO_PIN_0},
              {ButtonID::SEL, GPIOB, GPIO_PIN_1},
              {ButtonID::DOWN, GPIOB, GPIO_PIN_2}} {
  LedController::init();
  apply_state(State::MENU_MANUAL);
}

void UI::process() {
  for (int btn = ButtonID::UP; btn < ButtonID::COUNT; ++btn) {
    buttons[btn].process();
  }
  LedController::process();
}

void UI::setupLed(UI::State nestate) {
  static constexpr std::pair<LedController::Colors, LedController::blinkMode>
      stateLedModes[]{
          {LedController::Green, LedController::BLINK_NORMAL},
          {LedController::Red, LedController::BLINK_NORMAL},
          {LedController::Blue, LedController::BLINK_NORMAL},
          {},
          {LedController::Green, LedController::BLINK_NO},
          {LedController::Red, LedController::BLINK_NO},
          {LedController::Blue, LedController::BLINK_NO},
      };
  LedController::setColor(stateLedModes[nestate].first);
  LedController::setBlink(stateLedModes[nestate].second);
}

void UI::apply_state(State newstate) {
  const auto switch_state = [this](ButtonID btn) {
    State newstate;
    switch (btn) {
    case UP:
      newstate = static_cast<State>((this->state + 1) % MODE_COUNT);
      break;
    case DOWN:
      newstate = this->state == MENU_MANUAL
                     ? MENU_STEPDIR
                     : static_cast<State>(this->state - 1);
      break;
    default:
      assert(!"Unknown button pressed!");
    }
    apply_state(newstate);
  };

  const auto enter_mode = [this](ButtonID btn) {
    if (btn == SEL) {
      assert(this->state < MODE_COUNT);
      apply_state(static_cast<State>(this->state + MODE_COUNT + 1));
    }
  };

  const auto leave_mode = [this](ButtonID btn) {
    if (btn == SEL) {
      assert(this->state > MODE_COUNT);
      apply_state(static_cast<State>(this->state - MODE_COUNT - 1));
    }
  };

  switch (newstate) {
  case State::MENU_MANUAL:
  case State::MENU_PWM:
  case State::MENU_STEPDIR:
    buttons[ButtonID::UP].reset_callbacks();
    buttons[ButtonID::UP][BTN_ON_CLICK] = switch_state;
    buttons[ButtonID::SEL].reset_callbacks();
    buttons[ButtonID::SEL][BTN_ON_CLICK] = enter_mode;
    buttons[ButtonID::DOWN].reset_callbacks();
    buttons[ButtonID::DOWN][BTN_ON_CLICK] = switch_state;
    break;
  case State::MANUAL:
  case State::PWM:
  case State::STEP_DIR:
    buttons[ButtonID::UP].reset_callbacks();
    buttons[ButtonID::SEL].reset_callbacks();
    buttons[ButtonID::SEL][BTN_ON_CLICK] = leave_mode;
    buttons[ButtonID::DOWN].reset_callbacks();
    break;
  default:
    assert(!"Illegal UI state");
    break;
  }

  setupLed(newstate);

  this->state = newstate;
}
