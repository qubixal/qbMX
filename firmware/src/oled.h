#ifndef OLED_H
#define OLED_H

#include "config.h"
#include "pico/stdlib.h"

void oled_init(void);
void oled_task(uint32_t now_ms);
void oled_set_page(uint8_t page);
void oled_next_page(void);
void oled_set_wpm(uint16_t wpm);
void oled_set_layer_name(const char *name);
void oled_set_led_info(uint8_t mode, uint8_t brightness);
void oled_set_host_temps(float cpu, float gpu);
void oled_set_internal_temp(float temp);

#endif // OLED_H
