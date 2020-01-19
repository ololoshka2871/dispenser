#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <stm32f0xx_hal.h>

#include <functional>

#define BUTTON_ACTIVE_LVL 0
#define LONG_PUSH_PERIOD_MS 1500

enum callback_type {
  BTN_ON_PUSH = 0,
  BTN_ON_RELEASE = 1,
  BTN_ON_CLICK = 2,
  BTN_ON_LONG_PUSH = 3,
  COUNT = 4
};

template <typename Tid> struct Button {
  using callback_t = std::function<void(Tid)>;
  using this_type = Button<Tid>;

  enum state { RELEASED, PUSHED, LONG_PUSHED };

  Button(Tid id, GPIO_TypeDef *GPIO, uint16_t pin)
      : id(id), GPIO(GPIO), pin(pin), last_tick(), long_push_counter(),
        mystate(RELEASED) {
    GPIO_InitTypeDef GPIO_InitStruct = {pin, GPIO_MODE_INPUT, GPIO_NOPULL,
                                        GPIO_SPEED_FREQ_LOW};
    HAL_GPIO_Init(GPIO, &GPIO_InitStruct);
  }

  state getState() const { return mystate; }

  void process() {
    auto tick = HAL_GetTick();
    if (tick != last_tick) {
      auto value = HAL_GPIO_ReadPin(GPIO, pin);
      switch (mystate) {
      case RELEASED:
        if (value == BUTTON_ACTIVE_LVL) {
          // just bushed
          mystate = PUSHED;
          long_push_counter = 0;
          if (callbacks[BTN_ON_PUSH]) {
            callbacks[BTN_ON_PUSH](id);
          }
        }
        break;
      case PUSHED:
        if (value == BUTTON_ACTIVE_LVL) {
          // continue push
          if (long_push_counter == LONG_PUSH_PERIOD_MS) {
            mystate = LONG_PUSHED;
            if (callbacks[BTN_ON_LONG_PUSH]) {
              callbacks[BTN_ON_LONG_PUSH](id);
            }
          } else {
            ++long_push_counter;
          }
        } else {
          // just released
          mystate = RELEASED;
          if (callbacks[BTN_ON_RELEASE]) {
            callbacks[BTN_ON_RELEASE](id);
          }
          if (callbacks[BTN_ON_CLICK]) {
            callbacks[BTN_ON_CLICK](id);
          }
        }
        break;
      case LONG_PUSHED:
        if (value != BUTTON_ACTIVE_LVL) {
          // just released
          mystate = RELEASED;
          if (callbacks[BTN_ON_RELEASE]) {
            callbacks[BTN_ON_RELEASE](id);
          }
        }
        break;
      }

      last_tick = tick;
    }
  }

  callback_t operator[](const callback_type cbt) const {
    return callbacks[cbt];
  }

  callback_t &operator[](const callback_type cbt) { return callbacks[cbt]; }

private:
  Tid id;
  GPIO_TypeDef *GPIO;
  uint32_t pin;

  uint32_t last_tick;
  uint32_t long_push_counter;
  state mystate;

  callback_t callbacks[COUNT];
};

#endif /* _BUTTON_H_ */
