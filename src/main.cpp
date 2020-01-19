#include <stm32f0xx_hal.h>

#include "Button.h"
#include "LedController.h"

#include "BoardInit.h"

enum class ButtonID { UP = 0, SEL, DOWN };

void set_color(ButtonID id) {
  LedController::Colors c[]{LedController::Fuchsia, LedController::Lime,
                            LedController::Navy};
  LedController::setColor(c[(int)id]);
}

void set_color2(ButtonID id) {
  LedController::Colors c[]{LedController::Aqua, LedController::Olive,
                            LedController::Red};
  LedController::setColor(c[(int)id]);
}

extern "C" int main(void) {
  InitBoard();

  LedController::init();

  LedController::setColor(LedController::Yellow);
  LedController::setBlink(LedController::BLINK_SLOW);

  __HAL_RCC_GPIOB_CLK_ENABLE();

  Button<ButtonID> up{ButtonID::UP, GPIOB, GPIO_PIN_0},
      sel{ButtonID::SEL, GPIOB, GPIO_PIN_1},
      down{ButtonID::DOWN, GPIOB, GPIO_PIN_2};

  up[BTN_ON_CLICK] = set_color;
  sel[BTN_ON_CLICK] = set_color;
  down[BTN_ON_CLICK] = set_color;

  up[BTN_ON_LONG_PUSH] = set_color2;
  sel[BTN_ON_LONG_PUSH] = set_color2;
  down[BTN_ON_LONG_PUSH] = set_color2;

  for (;;) {
    LedController::process();

    up.process();
    sel.process();
    down.process();
  }
}
