#include "oled.h"
#include "ssd1306.h"
#include "config.h"
#include "hardware/i2c.h"
#include "font5x7.h"
#include <string.h>
#include <stdio.h>

// ── SSD1306 Low-Level ──────────────────────────────────────────

static ssd1306_t display;

static void ssd1306_send_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    i2c_write_blocking(display.i2c, display.addr, buf, 2, false);
}

static void ssd1306_init_hw(i2c_inst_t *i2c, uint8_t addr) {
    display.i2c = i2c;
    display.addr = addr;
    display.width = OLED_WIDTH;
    display.height = OLED_HEIGHT;

    uint8_t cmds[] = {
        0xAE, 0xD5, 0x80, 0xD9, 0xF1, 0xDA, 0x02,
        0xDB, 0x20, 0x8D, 0x14, 0x20, 0x00, 0x40,
        0xA1, 0xC8, 0x81, 0xCF, 0x2E, 0xAF,
    };
    for (int i = 0; i < sizeof(cmds); i++)
        ssd1306_send_cmd(cmds[i]);

    ssd1306_clear(&display);
    ssd1306_update(&display);
}

static void ssd1306_update(void) {
    ssd1306_send_cmd(0x21); ssd1306_send_cmd(0); ssd1306_send_cmd(OLED_WIDTH - 1);
    ssd1306_send_cmd(0x22); ssd1306_send_cmd(0); ssd1306_send_cmd(3);
    i2c_write_blocking(display.i2c, display.addr | 0x40,
                       display.buffer, sizeof(display.buffer), false);
}

// ── Drawing Primitives ─────────────────────────────────────────

static void clear(void) {
    memset(display.buffer, 0, sizeof(display.buffer));
}

static void set_pixel(uint8_t x, uint8_t y, bool on) {
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return;
    uint16_t idx = (y / 8) * OLED_WIDTH + x;
    uint8_t bit = (1 << (y & 7));
    if (on) display.buffer[idx] |= bit;
    else    display.buffer[idx] &= ~bit;
}

static void draw_char(uint8_t x, uint8_t y, char c) {
    if (c < 0x20 || c > 0x7E) return;
    const uint8_t *glyph = &font5x7[(c - 0x20) * 5];
    for (int col = 0; col < 5; col++) {
        uint8_t line = glyph[col];
        for (int row = 0; row < 7; row++)
            set_pixel(x + col, y + row, (line >> row) & 1);
    }
}

static void draw_string(uint8_t x, uint8_t y, const char *str) {
    while (*str && x < OLED_WIDTH - 5) {
        draw_char(x, y, *str++);
        x += 6;
    }
}

static void draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool on) {
    for (uint8_t i = 0; i < w; i++)
        for (uint8_t j = 0; j < h; j++)
            set_pixel(x + i, y + j, on);
}

static void draw_bar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t pct) {
    draw_rect(x, y, w, h, false);
    uint8_t fill = (uint16_t)w * pct / 100;
    draw_rect(x, y, fill, h, true);
    draw_rect(x, y, w, h, false);
    for (uint8_t i = 0; i < w; i++)
        set_pixel(x + i, y, true);
    for (uint8_t i = 0; i < w; i++)
        set_pixel(x + i, y + h - 1, true);
    for (uint8_t j = 0; j < h; j++) {
        set_pixel(x, y + j, true);
        set_pixel(x + w - 1, y + j, true);
    }
    draw_rect(x + 1, y + 1, fill - 1, h - 2, true);
}

// ── OLED Page Manager ──────────────────────────────────────────

static uint8_t current_page = 0;
static uint32_t last_oled_ms = 0;
static uint16_t cached_wpm = 0;
static const char *cached_layer = "QWERTY";
static uint8_t cached_led_mode = 0;
static uint8_t cached_led_brightness = 128;
static float cached_host_cpu = -1.0f;
static float cached_host_gpu = -1.0f;
static float cached_internal_temp = -1.0f;

static void page_boot_logo(void) {
    clear();
    draw_string(20, 4, "qbMX");
    draw_string(8, 16, "Keyboard v1.0");
    ssd1306_update();
}

static void page_layout(void) {
    clear();
    draw_string(0, 0, "-- LAYOUT --");
    draw_string(0, 12, cached_layer);
    ssd1306_update();
}

static void page_custom(void) {
    clear();
    draw_string(0, 4, "qbMX Keyboard");
    draw_string(0, 16, "Hack Club Keeb");
    ssd1306_update();
}

static void page_led_status(void) {
    clear();
    draw_string(0, 0, "-- LED --");
    const char *mode = (cached_led_mode == 0) ? "Warm White" : "RGB Cycle";
    draw_string(0, 12, mode);
    char buf[20];
    snprintf(buf, sizeof(buf), "Bright: %d%%", cached_led_brightness * 100 / 255);
    draw_string(0, 24, buf);
    ssd1306_update();
}

static void page_wpm(void) {
    clear();
    draw_string(0, 0, "-- WPM --");
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", cached_wpm);
    draw_string(30, 10, buf);
    uint8_t bar_pct = cached_wpm > 200 ? 100 : (cached_wpm * 100 / 200);
    draw_bar(10, 24, 108, 6, bar_pct);
    ssd1306_update();
}

static void page_host_temps(void) {
    clear();
    draw_string(0, 0, "-- HOST --");
    char buf[32];
    if (cached_host_cpu >= 0) {
        snprintf(buf, sizeof(buf), "CPU AVG: %5.1f C", cached_host_cpu);
    } else {
        snprintf(buf, sizeof(buf), "CPU AVG:   N/A");
    }
    draw_string(0, 9, buf);
    if (cached_host_gpu >= 0) {
        snprintf(buf, sizeof(buf), "GPU AVG: %5.1f C", cached_host_gpu);
    } else {
        snprintf(buf, sizeof(buf), "GPU AVG:   N/A");
    }
    draw_string(0, 18, buf);
    if (cached_internal_temp >= 0) {
        snprintf(buf, sizeof(buf), "PICO:    %5.1f C", cached_internal_temp);
    } else {
        snprintf(buf, sizeof(buf), "PICO:      N/A");
    }
    draw_string(0, 27, buf);
    ssd1306_update();
}

// ── Public API ─────────────────────────────────────────────────

void oled_init(void) {
    i2c_init(OLED_I2C_INST, OLED_I2C_FREQ);
    gpio_set_function(OLED_SDA, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA);
    gpio_pull_up(OLED_SCL);

    ssd1306_init_hw(OLED_I2C_INST, OLED_I2C_ADDR);
    page_boot_logo();
}

void oled_task(uint32_t now_ms) {
    if (now_ms - last_oled_ms < OLED_INTERVAL_MS) return;
    last_oled_ms = now_ms;

    switch (current_page) {
        case 0: page_layout(); break;
        case 1: page_custom(); break;
        case 2: page_led_status(); break;
        case 3: page_wpm(); break;
        case 4: page_host_temps(); break;
    }
}

void oled_set_page(uint8_t page) {
    current_page = page % OLED_PAGE_COUNT;
}

void oled_next_page(void) {
    current_page = (current_page + 1) % OLED_PAGE_COUNT;
}

void oled_set_wpm(uint16_t wpm) { cached_wpm = wpm; }
void oled_set_layer_name(const char *name) { cached_layer = name; }
void oled_set_led_info(uint8_t mode, uint8_t brightness) {
    cached_led_mode = mode;
    cached_led_brightness = brightness;
}

void oled_set_host_temps(float cpu, float gpu) {
    cached_host_cpu = cpu;
    cached_host_gpu = gpu;
}

void oled_set_internal_temp(float temp) {
    cached_internal_temp = temp;
}
