#include <stm32f0xx_hal_gpio.h>

#include "EXTI_manager.h"
#include "FreeRunningAccelStepper.h"

#include "StepDirDriver.h"

#define STEP_port GPIOA
#define STEP_pin GPIO_PIN_15

#define DIR_port GPIOB
#define DIR_pin GPIO_PIN_3

#define STEP_INTERRUPT_IRQn EXTI4_15_IRQn
#define STEP_IRQ_HEADER EXTI4_15_IRQHandler

static StepDirDriver *inst;

StepDirDriver::StepDirDriver(FreeRunningAccelStepper &stepper,
                             EXTI_manager_base &exti_manager, bool invert_dir)
    : AbstractStepDriver{stepper},
      exti_manager{exti_manager}, step_ex{[this]() { doStep(); }},
      Exti_manager{exti_manager}, invert_dir{invert_dir} {

  GPIO_InitTypeDef step_pin{STEP_pin, GPIO_MODE_IT_FALLING, GPIO_NOPULL,
                            GPIO_SPEED_FREQ_LOW};

  GPIO_InitTypeDef dir_pin{DIR_pin, GPIO_MODE_INPUT, GPIO_NOPULL,
                           GPIO_SPEED_FREQ_LOW};

  HAL_GPIO_Init(DIR_port, &dir_pin);
  exti_manager.RegisterCallback(&step_ex, STEP_port, step_pin);
}

StepDirDriver::~StepDirDriver() {
  exti_manager.UnRegisterCallback(STEP_port, STEP_pin);
  HAL_GPIO_DeInit(DIR_port, DIR_pin);
}

AbstractStepDriver &StepDirDriver::setEnabled(bool enable) {
  exti_manager.EnableCallback(STEP_pin, enable);
  NVIC_EnableIRQ(STEP_INTERRUPT_IRQn);

  return AbstractStepDriver::setEnabled(enable);
}

void StepDirDriver::begin(FreeRunningAccelStepper &stepper,
                          EXTI_manager_base &exti_manager, bool invert_dir) {
  if (inst) {
    delete inst;
  }
  inst = new StepDirDriver(stepper, exti_manager, invert_dir);
}

StepDirDriver &StepDirDriver::instance() { return *inst; }

void StepDirDriver::doStep() {
  auto direction = (!!HAL_GPIO_ReadPin(DIR_port, DIR_pin) ^ invert_dir)
                       ? FreeRunningAccelStepper::DIRECTION_CW
                       : FreeRunningAccelStepper::DIRECTION_CCW;

  stepper.doStep(direction);
}
