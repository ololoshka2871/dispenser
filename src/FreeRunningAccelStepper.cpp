#include <memory>

#include <UsTimer.h>

#include "FreeRunningAccelStepper.h"

#define Minimal_steps_to_stop 2

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
  if (Minimal_steps_to_stop > stepsToStop) {
    stepsToStop = Minimal_steps_to_stop;
  }
  move(dir == DIRECTION_CW ? stepsToStop * 2 : -stepsToStop * 2);
  free_run = true;
}

void FreeRunningAccelStepper::stop() {
  free_run = false;
  AccelStepper::stop();
}

void FreeRunningAccelStepper::stopHard() {
  free_run = false;

  _stepInterval = 0;
  _speed = 0.0;
  _n = 0;
}

void FreeRunningAccelStepper::run() {
  auto running = AccelStepper::run();
  if (running && free_run) {
    _targetPos =
        _currentPos + ((_currentPos < _targetPos) ? stepsToStop : -stepsToStop);
  }
}

float FreeRunningAccelStepper::getAcceleration() const { return _acceleration; }

void FreeRunningAccelStepper::doStep(Direction dir) {
  if (_direction == DIRECTION_CW) {
    // Clockwise
    _currentPos += 1;
  } else {
    // Anticlockwise
    _currentPos -= 1;
  }
  step(_currentPos);

  _lastStepTime = UsTimer::Instance().read_us();
}
