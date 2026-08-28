#ifndef CONFIG_H
#define CONFIG_H

#include "pico/stdlib.h"

// ── Matrix pins ──────────────────────────────────────────────
#define MATRIX_ROWS     6
#define MATRIX_COLS     15

#define ROW_0           0
#define ROW_1           1
#define ROW_2           2
#define ROW_3           3
#define ROW_4           4
#define ROW_5           5

#define COL_0           6
#define COL_1           7
#define COL_2           8
#define COL_3           9
#define COL_4           10
#define COL_5           11
#define COL_6           12
#define COL_7           13
#define COL_8           14
#define COL_9           15
#define COL_10          16
#define COL_11          17
#define COL_12          18
#define COL_13          19
#define COL_14          20

// ── Encoder pins ─────────────────────────────────────────────
#define ENCODER_A       21
#define ENCODER_B       22
#define ENC_A           ENCODER_A
#define ENC_B           ENCODER_B

// ── OLED I2C ─────────────────────────────────────────────────
#define OLED_I2C        i2c1
#define OLED_I2C_INST   i2c1
#define OLED_SDA        26
#define OLED_SCL        27
#define OLED_ADDR       0x3C
#define OLED_I2C_ADDR   0x3C
#define OLED_I2C_FREQ   400000
#define OLED_WIDTH      128
#define OLED_HEIGHT     32

// ── SK6812 LEDs ──────────────────────────────────────────────
#define LED_DATA_PIN    28
#define LED_COUNT       15
#define LED_PIO         pio0
#define LED_SM          0

// ── Timing ───────────────────────────────────────────────────
#define SCAN_INTERVAL_US    1000    // 1ms for matrix scan
#define DEBOUNCE_MS         5       // 5ms debounce
#define OLED_INTERVAL_MS    100     // 10Hz
#define LED_UPDATE_MS       16      // 60fps LED
#define WPM_WINDOW_MS       5000    // 5-second WPM window

// ── USB ──────────────────────────────────────────────────────
#define USB_VID          0xCafe
#define USB_PID          0x4005
#define USB_MANUF        "qbMX"
#define USB_MANUFACTURER "qbMX"
#define USB_PRODUCT      "qbMX Keyboard"

// ── Defaults ─────────────────────────────────────────────────
#define DEFAULT_BRIGHTNESS    128
#define DEFAULT_LED_BRIGHTNESS 128
#define DEFAULT_LED_MODE      0     // 0 = warm white, 1 = RGB
#define MAX_BRIGHTNESS        255
#define WARM_WHITE_R          255
#define WARM_WHITE_G          180
#define WARM_WHITE_B          100

// ── OLED pages ───────────────────────────────────────────────
#define OLED_PAGE_COUNT  5
#define PAGE_LAYOUT      0
#define PAGE_CUSTOM      1
#define PAGE_LED         2
#define PAGE_WPM         3
#define PAGE_HOST        4

#endif // CONFIG_H
