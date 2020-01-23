#ifndef _EXTI_MANAGER_H_
#define _EXTI_MANAGER_H_

#include <stm32f0xx_hal.h>

#include <functional>

struct EXTI_manager_base {
  using header_t = std::function<void()>;

  EXTI_manager_base() : enabled{0}, headers{} {}

  void RegisterCallback(const header_t *header, GPIO_TypeDef *GPIO,
                        GPIO_InitTypeDef &config) {
    headers[pin2Num(config.Pin)] = header;
    enabled |= config.Pin;

    HAL_GPIO_Init(GPIO, &config);
  }

  void UnRegisterCallback(GPIO_TypeDef *GPIO, uint16_t PIN) {
    HAL_GPIO_DeInit(GPIO, PIN);

    enabled &= ~PIN;
    headers[pin2Num(PIN)] = nullptr;
  }

  void EnableCallback(uint16_t PIN, bool enabled = true) {
    auto pinn = pin2Num(PIN);
    auto mask_en = ((uint32_t)enabled) << pinn;
    auto mask_dis = ((uint32_t)!enabled) << pinn;
    MODIFY_REG(this->enabled, mask_en, mask_dis);
  }

  void Trigger() {
    auto triggered = __HAL_GPIO_EXTI_GET_IT(GPIO_PIN_All) & enabled;
    if (triggered) {
      for (uint8_t i = 0; i < std::size(headers); i++) {
        if (triggered & (1 << i)) {
          (*headers[i])();
        }
      }
    }
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_All);
  }

protected:
  uint16_t enabled;
  const header_t *headers[16];

  uint8_t pin2Num(uint16_t PIN) {
    switch (PIN) {
    case GPIO_PIN_0:
      return 0;
    case GPIO_PIN_1:
      return 1;
    case GPIO_PIN_2:
      return 2;
    case GPIO_PIN_3:
      return 3;
    case GPIO_PIN_4:
      return 4;
    case GPIO_PIN_5:
      return 5;
    case GPIO_PIN_6:
      return 6;
    case GPIO_PIN_7:
      return 7;
    case GPIO_PIN_8:
      return 8;
    case GPIO_PIN_9:
      return 9;
    case GPIO_PIN_10:
      return 10;
    case GPIO_PIN_11:
      return 11;
    case GPIO_PIN_12:
      return 12;
    case GPIO_PIN_13:
      return 13;
    case GPIO_PIN_14:
      return 14;
    case GPIO_PIN_15:
      return 15;
    }
  }
};

#define define_EXTI_manager(name)                                              \
  struct EXTI##name##manager;                                                  \
  static EXTI##name##manager *EXTI##name##manager_inst = nullptr;              \
                                                                               \
  struct EXTI##name##manager : public EXTI_manager_base {                      \
    static void begin(uint32_t priority) {                                     \
      NVIC_SetPriority(EXTI##name##n, priority);                               \
      EXTI##name##manager_inst = new EXTI##name##manager();                    \
    }                                                                          \
                                                                               \
    static EXTI_manager_base &instance() { return *EXTI##name##manager_inst; } \
                                                                               \
  private:                                                                     \
    EXTI##name##manager() : EXTI_manager_base() {}                             \
  };                                                                           \
                                                                               \
  extern "C" void EXTI##name##Handler(void) {                                  \
    EXTI##name##manager::instance().Trigger();                                 \
  }

#endif /* _EXTI_MANAGER_H_ */
