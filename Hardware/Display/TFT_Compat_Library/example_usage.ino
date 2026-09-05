// Example: Using tft_compat.h with Lopaka-generated code
// This demonstrates how to integrate the compatibility library

#include "DEV_Config.h"
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "fonts.h"
#include "tft_compat.h"

// ============================================================
// STEP 1: Paste your Lopaka-generated code here, unchanged
// ============================================================

// [BEGIN lopaka generated]
static const unsigned char PROGMEM image_icon_bits[] = {
    0x00,0x00,0x3c,0x00,0x42,0x00,0x81,0x00,0x81,0x00,
    0x81,0x00,0x42,0x00,0x3c,0x00
};

void drawHomeScreen(void) {
    tft.fillScreen(0x0801);  // Near-black background

    // Title
    tft.setTextColor(0xE33F);  // Bright magenta accent
    tft.setTextSize(2);
    tft.drawString("Gym Assistant", 50, 40);

    // Icon
    tft.drawBitmap(120, 80, image_icon_bits, 16, 8, 0xE33F);

    // Body text
    tft.setTextColor(0xFFFF);  // White
    tft.setTextSize(1);
    tft.drawString("Ready to log workout", 60, 120);

    // Divider line
    tft.drawLine(20, 150, 260, 150, 0x6870);

    // Selection indicator (ellipse approximated as circle)
    tft.drawEllipse(140, 180, 8, 6, 0xE33F);
}
// [END lopaka generated]

// ============================================================
// STEP 2: Initialize hardware and call your draw function
// ============================================================

void setup() {
    // Initialize SPI, GPIO, and display driver
    Config_Init();
    LCD_Init();

    // Set up logical canvas in landscape orientation
    // Physical panel: 240×280 (portrait)
    // Logical canvas: 280×240 (landscape) with ROTATE_90
    Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_90, WHITE);

    // Render your Lopaka screen
    drawHomeScreen();
}

void loop() {
    // Add your animation or state update logic here
    // For static screens, loop can remain empty
}

// ============================================================
// NOTES:
// ============================================================
// 1. The tft object is a global instance provided by tft_compat.h
// 2. All Lopaka drawing commands work without modification
// 3. Colors are 16-bit RGB565 hex values (e.g., 0xE33F for magenta)
// 4. Fonts auto-map: setTextSize(1)→Font12, setTextSize(2)→Font24
// 5. Bitmaps must be PROGMEM arrays (Lopaka exports them this way)
