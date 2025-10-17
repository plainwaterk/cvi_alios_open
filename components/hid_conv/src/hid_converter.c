#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hid_converter.h"
#include "hid_keycodes.h"

// ASCII to HID keycode mapping structure
typedef struct {
    char ascii;              // ASCII character
    uint8_t hid_keycode;     // Corresponding HID keycode
    uint8_t needs_shift;     // Whether Shift key is needed
} ascii_to_hid_t;

// ASCII to HID keycode mapping table
static const ascii_to_hid_t ascii_to_hid_map[] = {
    // Lowercase letters
    {'a', HID_KEY_A, 0}, {'b', HID_KEY_B, 0}, {'c', HID_KEY_C, 0},
    {'d', HID_KEY_D, 0}, {'e', HID_KEY_E, 0}, {'f', HID_KEY_F, 0},
    {'g', HID_KEY_G, 0}, {'h', HID_KEY_H, 0}, {'i', HID_KEY_I, 0},
    {'j', HID_KEY_J, 0}, {'k', HID_KEY_K, 0}, {'l', HID_KEY_L, 0},
    {'m', HID_KEY_M, 0}, {'n', HID_KEY_N, 0}, {'o', HID_KEY_O, 0},
    {'p', HID_KEY_P, 0}, {'q', HID_KEY_Q, 0}, {'r', HID_KEY_R, 0},
    {'s', HID_KEY_S, 0}, {'t', HID_KEY_T, 0}, {'u', HID_KEY_U, 0},
    {'v', HID_KEY_V, 0}, {'w', HID_KEY_W, 0}, {'x', HID_KEY_X, 0},
    {'y', HID_KEY_Y, 0}, {'z', HID_KEY_Z, 0},
    
    // Uppercase letters (requires Shift)
    {'A', HID_KEY_A, 1}, {'B', HID_KEY_B, 1}, {'C', HID_KEY_C, 1},
    {'D', HID_KEY_D, 1}, {'E', HID_KEY_E, 1}, {'F', HID_KEY_F, 1},
    {'G', HID_KEY_G, 1}, {'H', HID_KEY_H, 1}, {'I', HID_KEY_I, 1},
    {'J', HID_KEY_J, 1}, {'K', HID_KEY_K, 1}, {'L', HID_KEY_L, 1},
    {'M', HID_KEY_M, 1}, {'N', HID_KEY_N, 1}, {'O', HID_KEY_O, 1},
    {'P', HID_KEY_P, 1}, {'Q', HID_KEY_Q, 1}, {'R', HID_KEY_R, 1},
    {'S', HID_KEY_S, 1}, {'T', HID_KEY_T, 1}, {'U', HID_KEY_U, 1},
    {'V', HID_KEY_V, 1}, {'W', HID_KEY_W, 1}, {'X', HID_KEY_X, 1},
    {'Y', HID_KEY_Y, 1}, {'Z', HID_KEY_Z, 1},
    
    // Number keys
    {'1', HID_KEY_1, 0}, {'2', HID_KEY_2, 0}, {'3', HID_KEY_3, 0},
    {'4', HID_KEY_4, 0}, {'5', HID_KEY_5, 0}, {'6', HID_KEY_6, 0},
    {'7', HID_KEY_7, 0}, {'8', HID_KEY_8, 0}, {'9', HID_KEY_9, 0},
    {'0', HID_KEY_0, 0},
    
    // Symbols (no Shift needed)
    {'-', HID_KEY_MINUS, 0}, {'=', HID_KEY_EQUAL, 0},
    {'[', HID_KEY_LBRACKET, 0}, {']', HID_KEY_RBRACKET, 0},
    {'\\', HID_KEY_BACKSLASH, 0}, {';', HID_KEY_SEMICOLON, 0},
    {'\'', HID_KEY_QUOTE, 0}, {'`', HID_KEY_GRAVE, 0},
    {',', HID_KEY_COMMA, 0}, {'.', HID_KEY_DOT, 0},
    {'/', HID_KEY_SLASH, 0},
    
    // Symbols (requires Shift)
    {'!', HID_KEY_1, 1}, {'@', HID_KEY_2, 1}, {'#', HID_KEY_3, 1},
    {'$', HID_KEY_4, 1}, {'%', HID_KEY_5, 1}, {'^', HID_KEY_6, 1},
    {'&', HID_KEY_7, 1}, {'*', HID_KEY_8, 1}, {'(', HID_KEY_9, 1},
    {')', HID_KEY_0, 1}, {'_', HID_KEY_MINUS, 1}, {'+', HID_KEY_EQUAL, 1},
    {'{', HID_KEY_LBRACKET, 1}, {'}', HID_KEY_RBRACKET, 1},
    {'|', HID_KEY_BACKSLASH, 1}, {':', HID_KEY_SEMICOLON, 1},
    {'"', HID_KEY_QUOTE, 1}, {'~', HID_KEY_GRAVE, 1},
    {'<', HID_KEY_COMMA, 1}, {'>', HID_KEY_DOT, 1},
    {'?', HID_KEY_SLASH, 1},
    
    // Special keys
    {' ', HID_KEY_SPACE, 0}, {'\t', HID_KEY_TAB, 0},
    {'\n', HID_KEY_ENTER, 0}, {'\r', HID_KEY_ENTER, 0},
    {'\b', HID_KEY_BACKSPACE, 0}
};

// Mapping table size
#define ASCII_TO_HID_MAP_SIZE (sizeof(ascii_to_hid_map) / sizeof(ascii_to_hid_map[0]))

// Get HID keycode and modifier info for an ASCII character
static int get_hid_from_ascii(char ascii, uint8_t *keycode, uint8_t *modifiers) {
    if (keycode == NULL || modifiers == NULL) {
        return -1; // Parameter error
    }

    // Default values
    *keycode = 0;
    *modifiers = 0;

    // Find in mapping table
    for (size_t i = 0; i < ASCII_TO_HID_MAP_SIZE; i++) {
        if (ascii_to_hid_map[i].ascii == ascii) {
            *keycode = ascii_to_hid_map[i].hid_keycode;
            *modifiers = ascii_to_hid_map[i].needs_shift ? HID_MOD_LSHIFT : 0;
            return 0; // Success
        }
    }

    return -1; // Not found
}

// Initialize HID report
static void init_hid_report(hid_keyboard_report_t *report) {
    if (report == NULL) {
        return;
    }
    
    report->modifiers = 0;
    report->reserved = 0;
    
    for (int i = 0; i < 6; i++) {
        report->keys[i] = 0;
    }
}

// Convert barcode string to HID keyboard report sequence
int barcode_to_hid(const char *barcode, barcode_hid_result_t *result) {
    if (barcode == NULL || result == NULL) {
        return -1; // Parameter error
    }

    // Initialize result
    result->reports = NULL;
    result->report_count = 0;
    
    size_t len = strlen(barcode);
    if (len == 0) {
        return 0; // Empty string, no conversion needed
    }
    
    // Each character needs two reports: press and release
    // Add Enter key if needed
    int need_enter = 1; // Default: add Enter
    int total_reports = len * 2;
    if (need_enter) {
        total_reports += 2;
    }
    
    // Allocate memory for reports
    result->reports = (hid_keyboard_report_t *)malloc(total_reports * sizeof(hid_keyboard_report_t));
    if (result->reports == NULL) {
        return -2; // Memory allocation failed
    }
    
    int report_index = 0;
    
    // Convert barcode characters
    for (size_t i = 0; i < len; i++) {
        uint8_t keycode = 0;
        uint8_t modifiers = 0;
        
        // Get HID keycode and modifiers for character
        if (get_hid_from_ascii(barcode[i], &keycode, &modifiers) != 0) {
            // Skip unsupported characters
            continue;
        }
        
        // Create key press report
        hid_keyboard_report_t *press_report = &(result->reports[report_index++]);
        init_hid_report(press_report);
        press_report->modifiers = modifiers;
        press_report->keys[0] = keycode;
        
        // Create key release report
        hid_keyboard_report_t *release_report = &(result->reports[report_index++]);
        init_hid_report(release_report);
    }
    
    // Add Enter key
    if (need_enter) {
        // Press Enter key
        hid_keyboard_report_t *enter_press = &(result->reports[report_index++]);
        init_hid_report(enter_press);
        enter_press->keys[0] = HID_KEY_ENTER;
        
        // Release Enter key
        hid_keyboard_report_t *enter_release = &(result->reports[report_index++]);
        init_hid_report(enter_release);
    }
    
    // Update actual report count
    result->report_count = report_index;
    
    return 0;
}

// Free conversion result
void free_barcode_hid_result(barcode_hid_result_t *result) {
    if (result == NULL) {
        return;
    }
    
    if (result->reports != NULL) {
        free(result->reports);
        result->reports = NULL;
    }
    
    result->report_count = 0;
}

// Convert HID report to hex string
void hid_report_to_hex(const hid_keyboard_report_t *report, char *hex_str, size_t hex_str_size) {
    if (report == NULL || hex_str == NULL || hex_str_size < (HID_REPORT_SIZE * 3)) {
        return;
    }
    
    // Format as hex string
    snprintf(hex_str, hex_str_size, 
             "%02X %02X %02X %02X %02X %02X %02X %02X",
             report->modifiers, report->reserved,
             report->keys[0], report->keys[1], report->keys[2],
             report->keys[3], report->keys[4], report->keys[5]);
}