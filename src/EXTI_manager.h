#ifndef _EXTI_MANAGER_H_
#define _EXTI_MANAGER_H_

#include <stm32f0xx_hal.h>

#include <functional>

struct EXTI_manager_base {
  using header_t = std::function<void()>;

  EXTI_manager_base() : registred_pins{0}, headers{} {}

  void RegisterCallback(const header_t *header, GPIO_TypeDef *GPIO,
                        GPIO_InitTypeDef &config) {
    headers[config.Pin] = header;

    HAL_GPIO_Init(GPIO, &config);
  }

  void UnRegisterCallback(GPIO_TypeDef *GPIO, uint16_t PIN) {
    HAL_GPIO_DeInit(GPIO, PIN);

    registred_pins &= ~PIN;
    headers[PIN] = nullptr;
  }

  void EnableCallback(uint16_t PIN, bool enabled = true) {
    auto mask_en = ((uint32_t)enabled) << PIN;
    auto mask_dis = ((uint32_t)!enabled) << PIN;
    MODIFY_REG(registred_pins, mask_en, mask_dis);
  }

  void Trigger() {
    auto triggered = __HAL_GPIO_EXTI_GET_IT(GPIO_PIN_All) & registred_pins;
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
  uint16_t registred_pins;
  const header_t *headers[16];
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
