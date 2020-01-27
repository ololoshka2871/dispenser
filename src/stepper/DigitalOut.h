#ifndef _DigitalOut_h_
#define _DigitalOut_h_

#include <stm32f0xx_hal_gpio.h>

struct DigitalOut {
  DigitalOut() : GPIO(nullptr) {}

  /// Create a DigitalOut connected to the specified pin.
  DigitalOut(GPIO_TypeDef *GPIO, uint16_t pin);

  ~DigitalOut();

  DigitalOut(const DigitalOut &) = delete;
  DigitalOut(DigitalOut &&rr);

  /// Set the output, specified as 0 or 1 (int)
  void write(int value);

  /// Return the output setting, represented as 0 or 1 (int)
  int read();

  /// Return the output setting, represented as 0 or 1 (int)
  int is_connected() { return GPIO != nullptr; }

  /// A shorthand for write()
  DigitalOut &operator=(int value) {
    write(value);
    return *this;
  }

  /// A shorthand for write() using the assignment operator which copies the
  /// state from the DigitalOut argument. More...
  DigitalOut &operator=(DigitalOut &rhs) {
    write(rhs.read());
    return *this;
  }

  /// A shorthand for read() More...
  operator int() { return read(); }

private:
  GPIO_TypeDef *GPIO;
  uint16_t pin;
};

#endif /* _DigitalOut_h_ */
