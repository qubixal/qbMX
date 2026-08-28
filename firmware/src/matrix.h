#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"

typedef struct {
    uint8_t modifiers;
    uint8_t keys[6];
} hid_report_t;

void matrix_init(void);
void matrix_scan(void);
bool matrix_changed(void);
hid_report_t matrix_get_report(void);
uint8_t matrix_get_modifier(void);
uint8_t matrix_get_key(uint8_t index);
uint8_t matrix_get_layer(void);
bool matrix_key_pressed(uint8_t row, uint8_t col);
uint16_t matrix_get_wpm(void);

#endif // MATRIX_H
