// tft_compat.h — lets unmodified Lopaka-generated draw functions run on
// Waveshare's GUI_Paint/LCD_Driver library instead of TFT_eSPI
#pragma once
#include "GUI_Paint.h"
#include "LCD_Driver.h"
#include "fonts.h"

class TFTCompat {
public:
    void fillScreen(uint16_t color) {
        _bg = color;
        Paint_Clear(color);
    }

    void drawRect(int x, int y, int w, int h, uint16_t color) {
        Paint_DrawRectangle(x, y, x + w, y + h, color, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    }

    void fillRect(int x, int y, int w, int h, uint16_t color) {
        Paint_DrawRectangle(x, y, x + w, y + h, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }

    void setTextColor(uint16_t fg) {
        _fg = fg;
        _useBg = _bg;          // approximates "transparent" text background
    }
    void setTextColor(uint16_t fg, uint16_t bg) {
        _fg = fg;
        _useBg = bg;
    }

    void setTextSize(int size) {
      if (size <= 1) _font = &Font8;
        else _font = &Font16;
    }

    void drawString(const char* text, int x, int y) {
        Paint_DrawString_EN(x, y, text, _font, _useBg, _fg);
    }

    // Monochrome 1bpp bitmap, MSB-first, row-major — matches Lopaka's icon format
    void drawBitmap(int x, int y, const unsigned char* bitmap, int w, int h, uint16_t color) {
        int bytesPerRow = (w + 7) / 8;
        for (int row = 0; row < h; row++) {
            for (int col = 0; col < w; col++) {
                uint8_t b = pgm_read_byte(&bitmap[row * bytesPerRow + (col / 8)]);
                if (b & (0x80 >> (col % 8))) {
                    Paint_SetPixel(x + col, y + row, color);
                }
            }
        }
    }

private:
  uint16_t _bg = WHITE, _fg = BLACK, _useBg = WHITE;
  sFONT* _font = &Font8;
};

TFTCompat tft;
