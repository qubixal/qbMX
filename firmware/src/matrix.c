#include "matrix.h"
#include "config.h"
#include "hardware/gpio.h"

static const uint8_t row_pins[MATRIX_ROWS] = {
    ROW_0, ROW_1, ROW_2, ROW_3, ROW_4, ROW_5
};

static const uint8_t col_pins[MATRIX_COLS] = {
    COL_0, COL_1, COL_2, COL_3, COL_4, COL_5, COL_6,
    COL_7, COL_8, COL_9, COL_10, COL_11, COL_12, COL_13, COL_14
};

static uint8_t matrix_state[MATRIX_ROWS][MATRIX_COLS];
static uint8_t matrix_debounce[MATRIX_ROWS][MATRIX_COLS];
static uint8_t matrix_prev[MATRIX_ROWS][MATRIX_COLS];
static bool matrix_dirty;
static uint8_t current_row;

static const uint8_t keymap[MATRIX_ROWS][MATRIX_COLS] = {
    // Row 0: Esc F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12 F13 (14 keys + F13)
    { 0x29, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
      0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x68 },
    // Row 1: ` 1 2 3 4 5 6 7 8 9 0 - = Delete F14 (15 keys)
    { 0x35, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23,
      0x24, 0x25, 0x26, 0x27, 0x2D, 0x2E, 0x49, 0x69 },
    // Row 2: Tab Q W E R T Y U I O P [ ] \ F15 (15 keys)
    { 0x2B, 0x14, 0x1A, 0x08, 0x15, 0x17, 0x1C,
      0x18, 0x0C, 0x12, 0x13, 0x2F, 0x30, 0x31, 0x6A },
    // Row 3: Caps A S D F G H J K L ; ' Return F16 (14 keys + F16)
    { 0x39, 0x04, 0x16, 0x07, 0x09, 0x0A, 0x0B,
      0x0D, 0x0E, 0x0F, 0x33, 0x34, 0x28, 0x6B },
    // Row 4: LShift Z X C V B N M , . / RShift Up F17 (14 keys + F17)
    { 0xE1, 0x1D, 0x1B, 0x06, 0x19, 0x05, 0x11,
      0x10, 0x36, 0x37, 0x38, 0xE5, 0x52, 0x6C },
    // Row 5: Fn Ctrl Opt Cmd (Space 6.25u) Cmd Opt Ctrl Left Down Right F18 (13 keys + F18)
    { 0x00, 0xE0, 0xE2, 0xE3, 0x2C, 0xE7, 0xE6,
      0xE4, 0x00, 0x50, 0x51, 0x4F, 0x00, 0x6D }
};

static const uint8_t modifiers[MATRIX_ROWS][MATRIX_COLS] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x20, 0, 0 },
    { 0x00, 0x01, 0x04, 0x08, 0, 0x10, 0x04, 0x01, 0, 0, 0, 0, 0 }
};

void matrix_init(void) {
    current_row = 0;
    matrix_dirty = false;

    for (int r = 0; r < MATRIX_ROWS; r++) {
        gpio_init(row_pins[r]);
        gpio_set_dir(row_pins[r], GPIO_OUT);
        gpio_put(row_pins[r], 1);
    }

    for (int c = 0; c < MATRIX_COLS; c++) {
        gpio_init(col_pins[c]);
        gpio_set_dir(col_pins[c], GPIO_IN);
        gpio_pull_up(col_pins[c]);
    }

    for (int r = 0; r < MATRIX_ROWS; r++)
        for (int c = 0; c < MATRIX_COLS; c++) {
            matrix_state[r][c] = 0;
            matrix_debounce[r][c] = 0;
            matrix_prev[r][c] = 0;
        }
}

void matrix_scan(void) {
    gpio_put(row_pins[current_row], 0);
    busy_wait_us(5);

    for (int c = 0; c < MATRIX_COLS; c++) {
        uint8_t pressed = !gpio_get(col_pins[c]);

        if (pressed != matrix_state[current_row][c]) {
            if (matrix_debounce[current_row][c] < DEBOUNCE_COUNT) {
                matrix_debounce[current_row][c]++;
            } else {
                matrix_state[current_row][c] = pressed;
                matrix_debounce[current_row][c] = 0;
                matrix_dirty = true;
            }
        } else {
            matrix_debounce[current_row][c] = 0;
        }
    }

    gpio_put(row_pins[current_row], 1);

    current_row = (current_row + 1) % MATRIX_ROWS;
}

bool matrix_changed(void) {
    if (matrix_dirty) {
        matrix_dirty = false;
        return true;
    }
    return false;
}

uint8_t matrix_get_modifier(void) {
    uint8_t mod = 0;
    for (int r = 0; r < MATRIX_ROWS; r++)
        for (int c = 0; c < MATRIX_COLS; c++)
            if (matrix_state[r][c] && modifiers[r][c])
                mod |= modifiers[r][c];
    return mod;
}

uint8_t matrix_get_key(uint8_t index) {
    uint8_t count = 0;
    for (int r = 0; r < MATRIX_ROWS; r++)
        for (int c = 0; c < MATRIX_COLS; c++)
            if (matrix_state[r][c] && !modifiers[r][c]) {
                if (count == index)
                    return keymap[r][c];
                count++;
            }
    return 0;
}

uint8_t matrix_get_layer(void) {
    return 0;
}

bool matrix_key_pressed(uint8_t row, uint8_t col) {
    if (row < MATRIX_ROWS && col < MATRIX_COLS)
        return matrix_state[row][col];
    return false;
}

hid_report_t matrix_get_report(void) {
    hid_report_t report = {0};
    report.modifiers = matrix_get_modifier();

    uint8_t key_count = 0;
    for (int r = 0; r < MATRIX_ROWS && key_count < 6; r++)
        for (int c = 0; c < MATRIX_COLS && key_count < 6; c++)
            if (matrix_state[r][c] && !modifiers[r][c]) {
                report.keys[key_count++] = keymap[r][c];
            }

    return report;
}

static uint16_t wpm_counter = 0;
static uint32_t wpm_last_ms = 0;
static uint16_t wpm_word_count = 0;

uint16_t matrix_get_wpm(void) {
    uint32_t now_ms = time_us_32() / 1000;
    if (now_ms - wpm_last_ms >= 5000) {
        wpm_counter = wpm_word_count * 12;
        wpm_word_count = 0;
        wpm_last_ms = now_ms;
    }
    return wpm_counter;
}
