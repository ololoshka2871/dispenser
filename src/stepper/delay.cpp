#include "UsTimer.h"

#include "delay.h"

void delay_us(uint32_t us) {
  auto &timer = UsTimer::Instance();
  auto to = timer.read_us() + us;
  while (timer.read_us() != to)
    ;
}
