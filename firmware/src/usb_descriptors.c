#include "usb_descriptors.h"
#include "config.h"
#include "tusb.h"

enum {
    ITF_NUM_HID_KBD = 0,
    ITF_NUM_HID_CDC,
    ITF_NUM_TOTAL
};

enum {
    REPORT_ID_KEYBOARD = 1,
    REPORT_ID_CONSUMER = 2,
};

// HID Report Descriptors
static const uint8_t hid_report_desc[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(REPORT_ID_KEYBOARD)),
    TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(REPORT_ID_CONSUMER)),
};

// Device Descriptor
static const tusb_desc_device_t device_desc = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01,
};

// Configuration Descriptor
static const uint8_t config_desc[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, 400, 0x00),

    // HID Keyboard Interface
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, 0x03,
        sizeof(hid_report_desc), 0x81, 16, 1000),

    // HID CDC (for volume/consumer)
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_CDC, 0, 0x03,
        sizeof(hid_report_desc), 0x82, 16, 1000),
};

// String Descriptors
static const char *string_desc[] = {
    [0] = (const char[]){0x09, 0x04},  // English
    [1] = USB_MANUFACTURER,
    [2] = USB_PRODUCT,
    [3] = "0001",
};

static const char *utf16_desc(const char *str) {
    static uint16_t buf[32];
    int len = 0;
    while (*str && len < 31) {
        buf[++len] = *str++;
    }
    buf[0] = (TUSB_DESC_STRING << 8) | (2 * len + 2);
    return (const char *)buf;
}

// Callbacks
const uint8_t *tud_descriptor_device_cb(void) {
    return (const uint8_t *)&device_desc;
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    return config_desc;
}

const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;
    if (index == 0)
        return (const uint16_t *)utf16_desc(string_desc[0]);
    if (index < 4)
        return (const uint16_t *)utf16_desc(string_desc[index]);
    return NULL;
}

const uint8_t *tud_hid_descriptor_report_cb(uint8_t instance) {
    return hid_report_desc;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                                hid_report_type_t report_type,
                                uint8_t *buffer, uint16_t reqlen) {
    (void)instance; (void)report_id; (void)report_type;
    (void)buffer; (void)reqlen;
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                            hid_report_type_t report_type,
                            const uint8_t *buffer, uint16_t bufsize) {
    (void)instance; (void)report_id; (void)report_type;
    (void)buffer; (void)bufsize;
}

void usb_init(void) {
    tusb_init();
}

void usb_task(void) {
    tud_task();
}

void usb_hid_send_keyboard(uint8_t modifiers, const uint8_t keys[6]) {
    if (tud_hid_ready()) {
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifiers, (uint8_t *)keys);
    }
}

void usb_hid_send_consumer(uint16_t key) {
    if (tud_hid_ready()) {
        uint8_t report[2] = {key & 0xFF, (key >> 8) & 0xFF};
        tud_hid_report(REPORT_ID_CONSUMER, report, sizeof(report));
    }
}
