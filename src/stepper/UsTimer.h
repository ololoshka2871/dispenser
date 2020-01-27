#ifndef _MU_TIMER_H_
#define _MU_TIMER_H_

#include <cstdint>

struct UsTimer {
  static UsTimer &Instance();

  uint32_t read_us();

private:
  UsTimer();
};

#endif
