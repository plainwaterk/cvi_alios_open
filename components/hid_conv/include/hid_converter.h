#ifndef HID_CONVERTER_H
#define HID_CONVERTER_H

#include <stdint.h>
#include <stddef.h>

// HID report length (USB HID keyboard spec)
#define HID_REPORT_SIZE 8

// Modifier key masks
#define HID_MOD_LCTRL  0x01
#define HID_MOD_LSHIFT 0x02
#define HID_MOD_LALT   0x04
#define HID_MOD_LWIN   0x08
#define HID_MOD_RCTRL  0x10
#define HID_MOD_RSHIFT 0x20
#define HID_MOD_RALT   0x40
#define HID_MOD_RWIN   0x80

// HID keyboard report structure
typedef struct {
    uint8_t modifiers;    // Modifier keys (Ctrl, Shift, Alt, Win)
    uint8_t reserved;     // Reserved byte, normally 0
    uint8_t keys[6];      // Up to 6 simultaneous key presses
} hid_keyboard_report_t;

// Barcode result structure
typedef struct {
    hid_keyboard_report_t *reports;  // Array of HID reports
    int report_count;                // Number of reports
} barcode_hid_result_t;

// Convert barcode string to HID keyboard report sequence
// Returns 0 on success, negative on failure
int barcode_to_hid(const char *barcode, barcode_hid_result_t *result);

// Free conversion result
void free_barcode_hid_result(barcode_hid_result_t *result);

// Convert HID report to hex string
void hid_report_to_hex(const hid_keyboard_report_t *report, char *hex_str, size_t hex_str_size);

#endif // MINI_HID_CONVERTER_H