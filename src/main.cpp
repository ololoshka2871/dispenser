#include <stm32f0xx_hal.h>

#include "FreeRunningAccelStepper.h"
#include "ManualDriver.h"
#include "PWMDriver.h"
#include "StepDirDriver.h"
#include "StepDriverSelector.h"
#include "UI.h"

#include "BoardInit.h"

extern "C" int main(void) {
  InitBoard();

  __HAL_RCC_GPIOB_CLK_ENABLE();

  // Инициализируемся с последовательностью выводов IN1-IN3-IN2-IN4
  // для использования AccelStepper с 28BYJ-48
  FreeRunningAccelStepper stepper{
      AccelStepper::FULL4WIRE, DigitalOut{GPIOB, GPIO_PIN_7},
      DigitalOut{GPIOB, GPIO_PIN_5}, DigitalOut{GPIOB, GPIO_PIN_6},
      DigitalOut{GPIOB, GPIO_PIN_4}};

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(3000);

  ManualDriver::begin(stepper, 500);
  PWMDriver::begin(stepper);
  StepDirDriver::begin(stepper, false);

  StepDriverSelector selector{&ManualDriver::instance(), &PWMDriver::instance(),
                              &StepDirDriver::instance()};

  UI ui{selector, ManualDriver::instance()};

  for (;;) {
    ui.process();
    stepper.run();
  }
}
