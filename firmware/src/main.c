#include "config.h"
#include "matrix.h"
#include "usb_descriptors.h"
#include "oled.h"
#include "leds.h"
#include "encoder.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/timer.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    stdio_init_all();
    usb_init();
    matrix_init();
    oled_init();
    leds_init();
    encoder_init();

    adc_init();
    adc_set_temp_sensor_enabled(true);

    // Startup animation
    leds_startup_wave();

    // Tracking time
    uint32_t last_scan_us = 0;
    uint32_t last_temp_ms = 0;

    // Encoder button state
    bool enc_btn_prev = false;
    uint32_t enc_btn_press_ms = 0;
    bool enc_btn_held = false;

    // Previous state for change detection
    hid_report_t prev_report = {0};
    bool first_report = true;

    while (1) {
        usb_task();

        // Parse incoming serial data from host
        int c;
        static char serial_buf[64];
        static uint8_t serial_idx = 0;
        while ((c = getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT) {
            if (c == '\n' || c == '\r') {
                serial_buf[serial_idx] = '\0';
                if (serial_idx > 0 && serial_buf[0] == 'T') {
                    // Format: T:<cpu_avg>:<gpu_avg>
                    float cpu, gpu;
                    if (sscanf(serial_buf, "T:%f:%f", &cpu, &gpu) == 2) {
                        oled_set_host_temps(cpu, gpu);
                    }
                }
                serial_idx = 0;
            } else if (serial_idx < sizeof(serial_buf) - 1) {
                serial_buf[serial_idx++] = (char)c;
            }
        }

        uint32_t now_us = time_us_32();
        uint32_t now_ms = now_us / 1000;

        // Matrix scan every 1ms
        if (now_us - last_scan_us >= SCAN_INTERVAL_US) {
            last_scan_us = now_us;
            matrix_scan();

            // Check for changes
            if (matrix_changed() || first_report) {
                hid_report_t report = matrix_get_report();

                // Send change on event
                if (report.modifiers != prev_report.modifiers ||
                    memcmp(report.keys, prev_report.keys, 6) != 0 ||
                    first_report) {
                    usb_hid_send_keyboard(report.modifiers, report.keys);
                    prev_report = report;
                    first_report = false;
                }
            }
        }

        // Encoder polling
        encoder_poll();
        int8_t enc_delta = encoder_get_delta();

        // Encoder button: read from matrix (Row 5, Col 13)
        bool enc_btn_now = matrix_key_pressed(5, 13);

        if (enc_btn_now && !enc_btn_prev) {
            // Button pressed
            enc_btn_press_ms = now_ms;
            enc_btn_held = false;
        } else if (enc_btn_now && !enc_btn_held) {
            // Button held - check for long press (3s)
            if (now_ms - enc_btn_press_ms >= 3000) {
                enc_btn_held = true;
                // Toggle encoder mode: 0=volume, 1=brightness
                uint8_t mode = encoder_get_mode();
                encoder_set_mode(mode ? 0 : 1);
            }
        } else if (!enc_btn_now && enc_btn_prev) {
            // Button released - short press = play/pause
            if (!enc_btn_held) {
                usb_hid_send_consumer(0x00CD); // Play/Pause
                sleep_ms(10);
                usb_hid_send_consumer(0x0000); // Release
            }
        }
        enc_btn_prev = enc_btn_now;

        // Encoder rotation
        if (enc_delta != 0) {
            if (encoder_get_mode() == 0) {
                // Volume mode
                if (enc_delta > 0) {
                    usb_hid_send_consumer(0x00E9); // Volume Up
                } else {
                    usb_hid_send_consumer(0x00EA); // Volume Down
                }
                sleep_ms(10);
                usb_hid_send_consumer(0x0000); // Release
            } else {
                // Brightness mode
                uint8_t bright = leds_get_brightness();
                int16_t new_bright = (int16_t)bright + (enc_delta * 16);
                if (new_bright < 0) new_bright = 0;
                if (new_bright > 255) new_bright = 255;
                leds_set_brightness((uint8_t)new_bright);
            }
        }

        leds_task(now_ms);
        oled_task(now_ms);

        // Update information
        if (now_ms - last_temp_ms >= 1000) {
            last_temp_ms = now_ms;
            adc_select_input(4);
            uint16_t raw = adc_read();
            float voltage = raw * 3.3f / 4095.0f;
            float pico_temp = 27.0f - (voltage - 0.706f) / 0.001721f;
            oled_set_internal_temp(pico_temp);
            oled_set_wpm(matrix_get_wpm());
            oled_set_led_info(leds_get_mode(), leds_get_brightness());
        }
    }

    return 0;
}
