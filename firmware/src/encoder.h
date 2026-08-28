#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <stdbool.h>

void encoder_init(void);
void encoder_poll(void);
int8_t encoder_get_delta(void);
bool encoder_button_pressed(void);

// Encoder mode: 0 = volume, 1 = brightness
void encoder_set_mode(uint8_t mode);
uint8_t encoder_get_mode(void);

#endif // ENCODER_H
