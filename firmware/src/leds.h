#ifndef LEDS_H
#define LEDS_H

#include "config.h"
#include "pico/stdlib.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} led_color_t;

void leds_init(void);
void leds_set_all(uint8_t r, uint8_t g, uint8_t b);
void leds_set_brightness(uint8_t brightness);
void leds_set_mode(uint8_t mode);
void leds_startup_wave(void);
void leds_update(void);
void leds_task(uint32_t now_ms);
void leds_trigger_press(uint8_t index);
void leds_trigger_release(uint8_t index);
led_color_t leds_get_color(void);
uint8_t leds_get_brightness(void);
uint8_t leds_get_mode(void);

#endif // LEDS_H
