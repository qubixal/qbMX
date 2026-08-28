#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define SSD1306_WIDTH  128
#define SSD1306_HEIGHT 32
#define SSD1306_BUFLEN (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

typedef struct {
    i2c_inst_t *i2c;
    uint8_t addr;
    uint8_t buffer[SSD1306_BUFLEN];
    uint8_t width;
    uint8_t height;
} ssd1306_t;

void ssd1306_command(ssd1306_t *dev, uint8_t cmd);
void ssd1306_data(ssd1306_t *dev, const uint8_t *data, uint16_t len);
void ssd1306_update(ssd1306_t *dev);
void ssd1306_clear(ssd1306_t *dev);
void ssd1306_set_pixel(ssd1306_t *dev, uint8_t x, uint8_t y, bool on);

#endif // SSD1306_H
