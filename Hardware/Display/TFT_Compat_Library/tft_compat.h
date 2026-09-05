// tft_compat.h — Complete TFT_eSPI compatibility shim for Waveshare LCD driver
// Lets unmodified Lopaka-generated draw functions render pixel-perfectly on
// Waveshare 1.83" LCD using GUI_Paint/LCD_Driver instead of TFT_eSPI

#pragma once
#include "GUI_Paint.h"
#include "LCD_Driver.h"
#include "fonts.h"

class TFTCompat {
public:
    // ========== Screen Management ==========

    void fillScreen(uint16_t color) {
        _bg = color;
        Paint_Clear(color);
    }

    // ========== Rectangle Drawing ==========

    void drawRect(int x, int y, int w, int h, uint16_t color) {
        // TFT_eSPI: (x, y, width, height)
        // Waveshare: (x1, y1, x2, y2)
        Paint_DrawRectangle(x, y, x + w - 1, y + h - 1, color, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    }

    void fillRect(int x, int y, int w, int h, uint16_t color) {
        Paint_DrawRectangle(x, y, x + w - 1, y + h - 1, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }

    // ========== Line Drawing ==========

    void drawLine(int x0, int y0, int x1, int y1, uint16_t color) {
        Paint_DrawLine(x0, y0, x1, y1, color, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    }

    // ========== Circle/Ellipse Drawing ==========

    // Lopaka uses drawEllipse for selection indicators
    // We approximate with circles since Waveshare doesn't have native ellipse support
    void drawEllipse(int x, int y, int rx, int ry, uint16_t color) {
        // Use the average of the two radii for a circular approximation
        int r = (rx + ry) / 2;
        if (r < 1) r = 1;
        Paint_DrawCircle(x, y, r, color, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    }

    void fillEllipse(int x, int y, int rx, int ry, uint16_t color) {
        int r = (rx + ry) / 2;
        if (r < 1) r = 1;
        Paint_DrawCircle(x, y, r, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }

    void drawCircle(int x, int y, int r, uint16_t color) {
        Paint_DrawCircle(x, y, r, color, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    }

    void fillCircle(int x, int y, int r, uint16_t color) {
        Paint_DrawCircle(x, y, r, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }

    // ========== Text Configuration ==========

    void setTextColor(uint16_t fg) {
        _fg = fg;
        _useBg = _bg;  // Transparent background (uses screen background)
    }

    void setTextColor(uint16_t fg, uint16_t bg) {
        _fg = fg;
        _useBg = bg;
    }

    // Map TFT_eSPI text sizes to Waveshare fonts
    // Lopaka typically uses size 1 (small) and size 2 (large)
    void setTextSize(int size) {
        switch (size) {
            case 1:  _font = &Font12; break;  // Small text
            case 2:  _font = &Font24; break;  // Large text (doubled)
            case 3:  _font = &Font24; break;  // Extra large (use Font24)
            default: _font = &Font16; break;  // Medium fallback
        }
    }

    // TFT_eSPI compatibility - does nothing but Lopaka code may call it
    void setFreeFont(const void* font = nullptr) {
        // Waveshare uses fixed fonts, ignore this call
        (void)font;
    }

    // ========== Text Drawing ==========

    void drawString(const char* text, int x, int y) {
        if (!text) return;
        Paint_DrawString_EN(x, y, text, _font, _useBg, _fg);
    }

    // ========== Bitmap Drawing ==========

    // Monochrome 1bpp bitmap, MSB-first, row-major — matches Lopaka's icon format
    // This is the format Lopaka exports: PROGMEM byte arrays
    void drawBitmap(int x, int y, const unsigned char* bitmap, int w, int h, uint16_t color) {
        if (!bitmap) return;

        int bytesPerRow = (w + 7) / 8;
        for (int row = 0; row < h; row++) {
            for (int col = 0; col < w; col++) {
                // Read byte from PROGMEM
                uint8_t b = pgm_read_byte(&bitmap[row * bytesPerRow + (col / 8)]);
                // Check if this pixel is set (MSB first)
                if (b & (0x80 >> (col % 8))) {
                    Paint_SetPixel(x + col, y + row, color);
                }
            }
        }
    }

    // ========== Utility Methods ==========

    // Get current text width (approximate - Waveshare doesn't provide this)
    int textWidth(const char* text) {
        if (!text) return 0;
        int len = strlen(text);
        // Approximate character width based on font
        // Font8: 5px, Font12: 7px, Font16: 11px, Font20: 14px, Font24: 17px
        int charWidth = 7;  // Default
        if (_font == &Font8)  charWidth = 5;
        if (_font == &Font12) charWidth = 7;
        if (_font == &Font16) charWidth = 11;
        if (_font == &Font20) charWidth = 14;
        if (_font == &Font24) charWidth = 17;
        return len * charWidth;
    }

    int fontHeight() {
        if (_font == &Font8)  return 8;
        if (_font == &Font12) return 12;
        if (_font == &Font16) return 16;
        if (_font == &Font20) return 20;
        if (_font == &Font24) return 24;
        return 12;  // Default
    }

    // ========== Direct Pixel Access ==========

    void drawPixel(int x, int y, uint16_t color) {
        Paint_SetPixel(x, y, color);
    }

    // ========== Advanced Drawing (for future compatibility) ==========

    void drawRoundRect(int x, int y, int w, int h, int r, uint16_t color) {
        // Approximate with regular rectangle for now
        drawRect(x, y, w, h, color);
    }

    void fillRoundRect(int x, int y, int w, int h, int r, uint16_t color) {
        // Approximate with regular filled rectangle for now
        fillRect(x, y, w, h, color);
    }

    // ========== Configuration Getters ==========

    uint16_t width() const {
        return Paint.Width;
    }

    uint16_t height() const {
        return Paint.Height;
    }

private:
    uint16_t _bg = 0xFFFF;     // Default white background
    uint16_t _fg = 0x0000;     // Default black foreground
    uint16_t _useBg = 0xFFFF;  // Actual background used (can be transparent)
    sFONT* _font = &Font12;    // Default to Font12 (reasonable for size 1)
};

// Global instance to match TFT_eSPI's API pattern
TFTCompat tft;
