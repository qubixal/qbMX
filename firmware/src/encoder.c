#include "encoder.h"
#include "config.h"
#include "hardware/gpio.h"

static volatile int8_t encoder_delta = 0;
static volatile uint8_t encoder_state = 0;
static volatile bool button_state = false;
static uint8_t encoder_mode = 0; // 0 = volume, 1 = brightness

static void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == ENC_A || gpio == ENC_B) {
        uint8_t a = gpio_get(ENC_A);
        uint8_t b = gpio_get(ENC_B);
        uint8_t new_state = (a << 1) | b;

        // Quadrature decoding
        switch (encoder_state) {
            case 0b00:
                if (new_state == 0b01) encoder_delta++;
                else if (new_state == 0b10) encoder_delta--;
                break;
            case 0b01:
                if (new_state == 0b11) encoder_delta++;
                else if (new_state == 0b00) encoder_delta--;
                break;
            case 0b11:
                if (new_state == 0b10) encoder_delta++;
                else if (new_state == 0b01) encoder_delta--;
                break;
            case 0b10:
                if (new_state == 0b00) encoder_delta++;
                else if (new_state == 0b11) encoder_delta--;
                break;
        }
        encoder_state = new_state;
    }
}

void encoder_init(void) {
    gpio_init(ENC_A);
    gpio_set_dir(ENC_A, GPIO_IN);
    gpio_pull_up(ENC_A);

    gpio_init(ENC_B);
    gpio_set_dir(ENC_B, GPIO_IN);
    gpio_pull_up(ENC_B);

    encoder_state = (gpio_get(ENC_A) << 1) | gpio_get(ENC_B);

    gpio_set_irq_enabled(ENC_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(ENC_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_add_raw_irq_handler(0, gpio_callback);
    irq_set_enabled(IO_IRQ_BANK0, true);
}

void encoder_poll(void) {
}

int8_t encoder_get_delta(void) {
    int32_t val = __atomic_exchange_n(&encoder_delta, 0, __ATOMIC_SEQ_CST);
    if (val > 3) val = 3;
    if (val < -3) val = -3;
    return (int8_t)val;
}

bool encoder_button_pressed(void) {
    return button_state;
}

void encoder_set_mode(uint8_t mode) {
    encoder_mode = mode;
}

uint8_t encoder_get_mode(void) {
    return encoder_mode;
}
