#include <stm32f0xx_hal.h>

#include <ws2812b.h>

#include "LedController.h"

static constexpr uint32_t blink_period_base = 250;

static LedController::Colors _color;

static LedController::blinkMode _blink;
static bool state;

static uint32_t blinkCounter;

static void set_color() {
  ws2812b_set(0, _color >> 16, _color >> 8, _color);
  ws2812b_send();
}

static void reset_color() {
  ws2812b_set(0, 0, 0, 0);
  ws2812b_send();
}

static void update_blink_counter() {
  switch (_blink) {
  case LedController::BLINK_FAST:
    blinkCounter = blink_period_base / 2;
    break;
  case LedController::BLINK_SLOW:
    blinkCounter = blink_period_base * 2;
    break;
  case LedController::BLINK_NORMAL:
  default:
    blinkCounter = blink_period_base;
    break;
  }
}

void LedController::init() {
  _blink = BLINK_NO;
  state = true;
  ws2812b_init();
  ws2812b_wait_ready(); //ждем, пока завершится инициализация

  reset_color();
}

void LedController::setColor(LedController::Colors color) {
  _color = color;
  ws2812b_wait_ready();
  if (state) {
    set_color();
  }
}

void LedController::setColor(uint8_t r, uint8_t g, uint8_t b) {
  _color = (LedController::Colors)(((uint32_t)r) << 16 | ((uint32_t)g) << 8 |
                                   ((uint32_t)b));
  ws2812b_wait_ready();
  if (state) {
    set_color();
  }
}

void LedController::setBlink(enum blinkMode blink) {
  _blink = blink;
  state = true;
}

void LedController::process() {
  if (_blink == BLINK_NO) {
    return;
  }

  static uint32_t prev_tich_counter = 0;

  auto curent_tick = HAL_GetTick();
  if (prev_tich_counter < curent_tick) {
    prev_tich_counter = curent_tick;

    if (blinkCounter > 0) {
      --blinkCounter;
      return;
    }
    update_blink_counter();

    state = !state;
    if (state) {
      set_color();
    } else {
      reset_color();
    }
  }
}
