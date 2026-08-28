#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

#include <stdint.h>
#include <stdbool.h>
#include "tusb.h"

void usb_init(void);
void usb_task(void);
void usb_hid_send_keyboard(uint8_t modifiers, const uint8_t keys[6]);
void usb_hid_send_consumer(uint16_t key);

#endif // USB_DESCRIPTORS_H
