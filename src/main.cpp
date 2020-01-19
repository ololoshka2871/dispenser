#include <stm32f0xx_hal.h>

#include "LedController.h"

#include "BoardInit.h"

extern "C" int main(void) {
  InitBoard();

  LedController::init();

  LedController::setColor(LedController::Yellow);
  LedController::setBlink(LedController::BLINK_SLOW);

  while (1) {
    LedController::process();
  }
}
