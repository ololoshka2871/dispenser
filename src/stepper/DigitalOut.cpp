#include <memory>

#include "DigitalOut.h"

DigitalOut::DigitalOut(GPIO_TypeDef *GPIO, uint16_t pin)
    : GPIO(GPIO), pin(pin) {
  GPIO_InitTypeDef init{pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,
                        GPIO_SPEED_FREQ_MEDIUM, 0};
  HAL_GPIO_Init(GPIO, &init);
}

DigitalOut::~DigitalOut() {
  if (GPIO) {
    HAL_GPIO_DeInit(GPIO, pin);
  }
}

DigitalOut::DigitalOut(DigitalOut &&rr)
    : GPIO(std::move(rr.GPIO)), pin(std::move(rr.pin)) {
  rr.GPIO = nullptr;
}

void DigitalOut::write(int value) {
  if (is_connected()) {
    HAL_GPIO_WritePin(GPIO, pin, static_cast<GPIO_PinState>(!!value));
  }
}

int DigitalOut::read() {
  return is_connected() ? !!HAL_GPIO_ReadPin(GPIO, pin) : 0;
}
