#include <memory>

#include <stm32f0xx_hal_gpio.h>

#include "FreeRunningAccelStepper.h"

#include "StepDirDriver.h"

#define STEP_port GPIOA
#define STEP_pin GPIO_PIN_15

#define DIR_port GPIOB
#define DIR_pin GPIO_PIN_3

#define STEP_INTERRUPT_IRQn EXTI4_15_IRQn
#define STEP_IRQ_HEADER EXTI4_15_IRQHandler

static std::unique_ptr<StepDirDriver> inst;

extern "C" void STEP_IRQ_HEADER() { HAL_GPIO_EXTI_IRQHandler(STEP_pin); }

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == STEP_pin) {
    StepDirDriver::instance().doStep();
  }
}

StepDirDriver::StepDirDriver(FreeRunningAccelStepper &stepper, bool invert_dir)
    : AbstractStepDriver{stepper}, invert_dir{invert_dir} {

  GPIO_InitTypeDef step_pin{STEP_pin, GPIO_MODE_IT_FALLING, GPIO_NOPULL,
                            GPIO_SPEED_FREQ_LOW};

  GPIO_InitTypeDef dir_pin{DIR_pin, GPIO_MODE_INPUT, GPIO_NOPULL,
                           GPIO_SPEED_FREQ_LOW};

  HAL_GPIO_Init(STEP_port, &step_pin);
  HAL_GPIO_Init(DIR_port, &dir_pin);

  NVIC_SetPriority(STEP_INTERRUPT_IRQn, 1);
}

StepDirDriver::~StepDirDriver() {
  NVIC_DisableIRQ(STEP_INTERRUPT_IRQn);

  HAL_GPIO_DeInit(STEP_port, STEP_pin);
  HAL_GPIO_DeInit(DIR_port, DIR_pin);
}

AbstractStepDriver &StepDirDriver::setEnabled(bool enable) {
  if (enable) {
    NVIC_EnableIRQ(STEP_INTERRUPT_IRQn);
  } else {
    NVIC_DisableIRQ(STEP_INTERRUPT_IRQn);
  }
  return AbstractStepDriver::setEnabled(enable);
}

void StepDirDriver::begin(FreeRunningAccelStepper &stepper, bool invert_dir) {
  inst.reset(new StepDirDriver(stepper, invert_dir));
}

StepDirDriver &StepDirDriver::instance() { return *inst; }

void StepDirDriver::doStep() {
  auto direction = (!!HAL_GPIO_ReadPin(DIR_port, DIR_pin) ^ invert_dir)
                       ? FreeRunningAccelStepper::DIRECTION_CW
                       : FreeRunningAccelStepper::DIRECTION_CCW;

  stepper.doStep(direction);
}
