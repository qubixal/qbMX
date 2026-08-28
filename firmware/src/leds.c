#include "leds.h"
#include "config.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include <string.h>
#include <math.h>

static PIO pio = LED_PIO;
static uint sm;

static uint8_t led_buffer[LED_COUNT][3]; // GRB
static uint8_t brightness = DEFAULT_BRIGHTNESS;
static uint8_t led_mode = DEFAULT_LED_MODE;
static bool leds_dirty = true;

// SK6812 PIO
// 6 instruction program for 800kHz output @ 125MHz
// Bit 0: T0H=240ns (3 cyc), total=320ns | Bit 1: T1H=640ns (8 cyc), total=720ns
static const uint16_t sk6812_instrs[] = {
    0x6020,  // 0: set y, 0
    0x80a0,  // 1: out x, 8
    0x6001,  // 2: set pins, 1
    0x0142,  // 3: jmp !x--, 2 [1]
    0x00c3,  // 4: jmp y--, 3 [6]
    0x6000,  // 5: set pins, 0
};
static const struct pio_program sk6812_prog = {
    .instructions = sk6812_instrs,
    .length = 6,
    .origin = -1,
};

static void led_push_buffer(void) {
    for (int i = 0; i < LED_COUNT; i++) {
        for (int j = 0; j < 3; j++) {
            uint8_t val = led_buffer[i][j];
            val = (uint16_t)val * brightness / 255;
            pio_sm_put_blocking(pio, sm, val);
        }
    }
    sleep_us(50); // latch (>50µs low)
}

// ── Color Helpers ────────────────────────────────────────────

static void set_all_rgb(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < LED_COUNT; i++) {
        led_buffer[i][0] = g; // SK6812 is GRB
        led_buffer[i][1] = r;
        led_buffer[i][2] = b;
    }
    leds_dirty = true;
}

static void hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b) {
    if (s == 0) { *r = *g = *b = v; return; }
    uint8_t region = h / 43;
    uint8_t remainder = (h - (region * 43)) * 6;
    uint8_t p = (v * (255 - s)) >> 8;
    uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
    switch (region) {
        case 0:  *r = v; *g = t; *b = p; break;
        case 1:  *r = q; *g = v; *b = p; break;
        case 2:  *r = p; *g = v; *b = t; break;
        case 3:  *r = p; *g = q; *b = v; break;
        case 4:  *r = t; *g = p; *b = v; break;
        default: *r = v; *g = p; *b = q; break;
    }
}

// ── Startup Wave ─────────────────────────────────────────────

static bool startup_active = false;
static uint32_t startup_start_ms = 0;

void leds_startup_wave(void) {
    startup_active = true;
    startup_start_ms = to_ms_since_boot(get_absolute_time());
}

static void startup_wave_step(uint32_t now_ms) {
    if (!startup_active) return;
    uint32_t elapsed = now_ms - startup_start_ms;
    if (elapsed > 1500) { startup_active = false; return; }

    uint8_t wave_pos = (uint32_t)LED_COUNT * elapsed / 1500;
    for (int i = 0; i < LED_COUNT; i++) {
        int dist = i - wave_pos;
        if (dist < -2 || dist > 2) {
            led_buffer[i][0] = led_buffer[i][1] = led_buffer[i][2] = 0;
        } else {
            uint8_t hue = (i * 17) & 0xFF;
            uint8_t r, g, b;
            hsv_to_rgb(hue, 255, 255, &r, &g, &b);
            led_buffer[i][0] = g;
            led_buffer[i][1] = r;
            led_buffer[i][2] = b;
        }
    }
    leds_dirty = true;
}

// ── RGB Cycle ────────────────────────────────────────────────

static uint8_t rgb_hue_offset = 0;

static void rgb_cycle_step(void) {
    rgb_hue_offset += 2;
    for (int i = 0; i < LED_COUNT; i++) {
        uint8_t hue = (rgb_hue_offset + i * 17) & 0xFF;
        uint8_t r, g, b;
        hsv_to_rgb(hue, 200, 255, &r, &g, &b);
        led_buffer[i][0] = g;
        led_buffer[i][1] = r;
        led_buffer[i][2] = b;
    }
    leds_dirty = true;
}

// ── Public API ───────────────────────────────────────────────

void leds_init(void) {
    uint offset = pio_add_program(pio, &sk6812_prog);
    sm = pio_claim_unused_sm(pio, true);

    // SM Config
    // out - bit 0, set on LED_DATA_PIN
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + 0, offset + 5);
    sm_config_set_out(&c, &pio->gpio, 1, false, false, false, false);
    sm_config_set_set(&c, &pio->gpio, 1, false);
    sm_config_set_clkdiv(&c, 1.0f); // 125MHz
    pio_sm_init(pio, sm, offset, &c);

    // Config LED_DATA_PIN as out
    pio_gpio_init(pio, LED_DATA_PIN);
    pio_sm_set_consecutive_pindirs(pio, sm, LED_DATA_PIN, 1, true);
    pio_sm_set_set_pin(pio, sm, LED_DATA_PIN);

    pio_sm_set_enabled(pio, sm, true);

    memset(led_buffer, 0, sizeof(led_buffer));
    set_all_rgb(WARM_WHITE_R, WARM_WHITE_G, WARM_WHITE_B);
    led_push_buffer();
}

void leds_set_all(uint8_t r, uint8_t g, uint8_t b) {
    set_all_rgb(r, g, b);
}

void leds_set_brightness(uint8_t b) {
    brightness = b;
    leds_dirty = true;
}

void leds_set_mode(uint8_t mode) {
    led_mode = mode;
    leds_dirty = true;
}

void leds_task(uint32_t now_ms) {
    if (startup_active) {
        startup_wave_step(now_ms);
        if (leds_dirty) { led_push_buffer(); leds_dirty = false; }
        return;
    }

    if (led_mode == 1) {
        rgb_cycle_step();
    }

    if (leds_dirty) {
        led_push_buffer();
        leds_dirty = false;
    }
}

void leds_update(void) {
    leds_task(to_ms_since_boot(get_absolute_time()));
}

led_color_t leds_get_color(void) {
    return (led_color_t){led_buffer[0][1], led_buffer[0][0], led_buffer[0][2]};
}

uint8_t leds_get_brightness(void) { return brightness; }
uint8_t leds_get_mode(void) { return led_mode; }
void leds_trigger_press(uint8_t idx) { (void)idx; }
void leds_trigger_release(uint8_t idx) { (void)idx; }
