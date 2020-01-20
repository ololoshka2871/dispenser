#include <stm32f0xx_hal.h>

#include "UI.h"

#include "BoardInit.h"

extern "C" int main(void) {
  InitBoard();

  __HAL_RCC_GPIOB_CLK_ENABLE();

  UI ui;

  for (;;) {
    ui.process();
  }
}
