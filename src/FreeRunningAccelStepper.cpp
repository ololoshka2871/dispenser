#include <memory>

#include "FreeRunningAccelStepper.h"

FreeRunningAccelStepper::FreeRunningAccelStepper(uint8_t interface,
                                                 DigitalOut &&pin1,
                                                 DigitalOut &&pin2,
                                                 DigitalOut &&pin3,
                                                 DigitalOut &&pin4, bool enable)
    : AccelStepper(interface, std::move(pin1), std::move(pin2), std::move(pin3),
                   std::move(pin4), enable),
      free_run{false} {}

void FreeRunningAccelStepper::moveFree(Direction dir) {
  stepsToStop = (long)((_maxSpeed * _maxSpeed) / (2.0 * _acceleration)) + 1;
  move(dir == DIRECTION_CW ? stepsToStop * 2 : -stepsToStop * 2);
  free_run = true;
}

void FreeRunningAccelStepper::stop() {
  free_run = false;
  AccelStepper::stop();
}

void FreeRunningAccelStepper::run() {
  auto running = AccelStepper::run();
  if (running && free_run) {
    _targetPos = _currentPos + stepsToStop;
  }
}
