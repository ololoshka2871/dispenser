#include <stm32f0xx_hal.h>

#include <ws2812b.h>

#include "BoardInit.h"

extern "C" int main(void) {
  InitBoard();

  ws2812b_init();
  ws2812b_wait_ready(); //ждем, пока завершится инициализация

  ws2812b_set(0, 0xFF, 0x00, 0x00);
  ws2812b_send(); //отправляем буфер из ОЗУ в светодиоды

  while (1) {
    HAL_Delay(100);
  }
}
