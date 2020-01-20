#include <AccelStepper.h>
#include <stm32f0xx_hal.h>

#include "UI.h"

#include "BoardInit.h"

extern "C" int main(void) {
  InitBoard();

  __HAL_RCC_GPIOB_CLK_ENABLE();

  AccelStepper stepper{AccelStepper::HALF4WIRE, DigitalOut{GPIOB, GPIO_PIN_7},
                       DigitalOut{GPIOB, GPIO_PIN_6},
                       DigitalOut{GPIOB, GPIO_PIN_5},
                       DigitalOut{GPIOB, GPIO_PIN_4}};

  stepper.setSpeed(1000);
  stepper.setAcceleration(5000);

  UI ui;

  stepper.moveTo(500);

  for (;;) {
    ui.process();
    stepper.run();
  }
}
