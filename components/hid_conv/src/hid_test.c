#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hid_converter.h"

void test_barcode(const char *barcode) {
    printf("====== Barcode: \"%s\" ======\n", barcode);
    
    // Convert barcode to HID reports
    barcode_hid_result_t result;
    int ret = barcode_to_hid(barcode, &result);
    
    if (ret != 0) {
        printf("Conversion failed, error code: %d\n", ret);
        return;
    }
    
    printf("Generated HID reports: %d\n\n", result.report_count);
    
    // Print HID reports
    for (int i = 0; i < result.report_count; i++) {
        char hex_str[32];
        hid_report_to_hex(&result.reports[i], hex_str, sizeof(hex_str));
        printf("Report #%03d: %s  (%s)\n", 
               i + 1, 
               hex_str, 
               (i % 2 == 0) ? "press" : "release");
    }
    
    // Generate raw binary format (for direct USB HID device transmission)
    printf("\nRaw binary data (hex representation, 8 bytes per report):\n");
    for (int i = 0; i < result.report_count; i++) {
        const hid_keyboard_report_t *report = &result.reports[i];
        printf("%02X%02X%02X%02X%02X%02X%02X%02X ", 
               report->modifiers, report->reserved,
               report->keys[0], report->keys[1], report->keys[2],
               report->keys[3], report->keys[4], report->keys[5]);
        
        if ((i + 1) % 2 == 0) printf("\n");
    }
    
    printf("\n");
    
    // Free resources
    free_barcode_hid_result(&result);
}

int main(int argc, char *argv[]) {
    // With command line arguments
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            test_barcode(argv[i]);
            if (i < argc - 1) {
                printf("\n");
            }
        }
        return 0;
    }
    
    // Default test cases
    printf("=== Barcode to HID Virtual Key Conversion Test ===\n\n");
    
    // Test some typical barcodes
    test_barcode("123456789012"); // UPC-A
    printf("\n");
    
    test_barcode("ABC-123-xyz"); // Code 39 type
    printf("\n");
    
    test_barcode("9780321584991"); // ISBN-13

    return 0;
}