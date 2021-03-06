#include <stm32f0xx_hal.h>

#include "FreeRunningAccelStepper.h"
#include "ManualDriver.h"
#include "PWMDriver.h"
#include "StepDirDriver.h"
#include "StepDriverSelector.h"
#include "UI.h"

#include "EXTI_manager.h"

#include "BoardInit.h"

#include <iterator>

#define DEFAULT_SPEED 500

define_EXTI_manager(4_15_IRQ);

extern "C" int main(void) {
  InitBoard();

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  RCC->APB1ENR |= RCC_APB2ENR_DBGMCUEN;
  DBGMCU->APB1FZ |=
      DBGMCU_APB1_FZ_DBG_TIM3_STOP | DBGMCU_APB1_FZ_DBG_TIM14_STOP;

  EXTI4_15_IRQmanager::begin(1);

  auto &exti_mgr = EXTI4_15_IRQmanager::instance();

  // Инициализируемся с последовательностью выводов IN1-IN3-IN2-IN4
  // для использования AccelStepper с 28BYJ-48
  FreeRunningAccelStepper stepper{
      AccelStepper::FULL4WIRE,
      // forward
      DigitalOut{GPIOB, GPIO_PIN_4}, DigitalOut{GPIOB, GPIO_PIN_6},
      DigitalOut{GPIOB, GPIO_PIN_5}, DigitalOut{GPIOB, GPIO_PIN_7}

      // revers
      /*
      DigitalOut{GPIOB, GPIO_PIN_7},
      DigitalOut{GPIOB, GPIO_PIN_5},
      DigitalOut{GPIOB, GPIO_PIN_6},
      DigitalOut{GPIOB, GPIO_PIN_4}
      */
  };

  stepper.setMaxSpeed(DEFAULT_SPEED);
  stepper.setAcceleration(3000);

  ManualDriver::begin(stepper, DEFAULT_SPEED, 250);
  PWMDriver::begin(stepper, exti_mgr, DEFAULT_SPEED, 3000, DEFAULT_SPEED / 2);
  StepDirDriver::begin(stepper, exti_mgr, false);

  auto &pwmDriver = PWMDriver::instance();

  StepDriverSelector selector{&ManualDriver::instance(), &pwmDriver,
                              &StepDirDriver::instance()};

  UI ui{selector, ManualDriver::instance()};

  for (;;) {
    ui.process();
    stepper.run();
    pwmDriver.process();
  }
}
