#include <AccelStepper.h>
#include <stm32f0xx_hal.h>

#include "UI.h"

#include "BoardInit.h"

// ManualDriver manualDriver;
// PWMDriver pwmDriver;
// StepDirDriver stepdirDriver;

// DriverSelector selector{manualDriver, pwmDriver, stepdirDriver};

extern "C" int main(void) {
  InitBoard();

  __HAL_RCC_GPIOB_CLK_ENABLE();

  // Инициализируемся с последовательностью выводов IN1-IN3-IN2-IN4
  // для использования AccelStepper с 28BYJ-48
  AccelStepper stepper{AccelStepper::FULL4WIRE, DigitalOut{GPIOB, GPIO_PIN_7},
                       DigitalOut{GPIOB, GPIO_PIN_5},
                       DigitalOut{GPIOB, GPIO_PIN_6},
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
