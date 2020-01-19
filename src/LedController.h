#ifndef _LED_CONTROLLER_H_
#define _LED_CONTROLLER_H_

#include <cstdint>

struct LedController {
  enum Colors {
    White = 0xFFFFFF,
    Silver = 0xC0C0C0,
    Gray = 0x808080,
    Black = 0x000000,
    Red = 0xFF0000,
    Maroon = 0x800000,
    Yellow = 0xFFFF00,
    Olive = 0x808000,
    Lime = 0x00FF00,
    Green = 0x008000,
    Aqua = 0x00FFFF,
    Teal = 0x008080,
    Blue = 0x0000FF,
    Navy = 0x000080,
    Fuchsia = 0xFF00FF,
    Purple = 0x800080,
  };

  enum blinkMode { BLINK_NO, BLINK_FAST, BLINK_NORMAL, BLINK_SLOW };

  static void init();
  static void setColor(Colors color);
  static void setColor(uint8_t r, uint8_t g, uint8_t b);
  static void setBlink(enum blinkMode blink = BLINK_NORMAL);
  static void process();

private:
  LedController() {}
};

#endif /* _LED_CONTROLLER_H_ */
