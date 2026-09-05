# Lopaka-to-Waveshare LCD Compatibility Shim

This project demonstrates how to use **Lopaka-generated TFT_eSPI code** with the **Waveshare 1.83" LCD** (240×280, NV3030B driver) on Arduino-compatible platforms like the XIAO ESP32S3.

## The Problem

[Lopaka](https://lopaka.app/) is an excellent visual screen designer that generates Arduino code for TFT_eSPI library. However, the Waveshare 1.83" LCD doesn't work with TFT_eSPI out of the box—it requires the manufacturer's custom GUI_Paint/LCD_Driver library.

## The Solution

`tft_compat.h` provides a **drop-in compatibility layer** that lets you paste unmodified Lopaka-generated code and have it render correctly on the Waveshare LCD.

## Features

✅ **Complete API Coverage**
- `fillScreen()`, `drawRect()`, `fillRect()`
- `drawLine()` for dividers and borders
- `drawEllipse()`, `fillEllipse()` for selection indicators
- `drawCircle()`, `fillCircle()`
- `drawPixel()` for individual pixels
- `drawBitmap()` for monochrome icons (Lopaka's 1bpp PROGMEM format)

✅ **Text Rendering**
- `setTextColor(fg)` and `setTextColor(fg, bg)`
- `setTextSize(1)` → Font12, `setTextSize(2)` → Font24
- `drawString()` with proper positioning
- `setFreeFont()` stub for compatibility

✅ **Color Handling**
- Full 16-bit RGB565 color support
- Matches Lopaka's color palette exactly

✅ **Font Mapping**
- TFT_eSPI text size → Waveshare font mapping
- Available fonts: Font8, Font12, Font16, Font20, Font24

## Quick Start

### Hardware Setup

**XIAO ESP32S3 to Waveshare 1.83" LCD:**
```
LCD RST → D3 (GPIO4)
LCD DC  → D1 (GPIO2)
LCD CS  → D0 (GPIO1)  ⚠️ NOT D9 (conflicts with hardware MISO)
LCD DIN → D10 (GPIO9) (hardware SPI MOSI)
LCD CLK → D8 (GPIO7)  (hardware SPI SCK)
LCD BL  → 3V3 (or PWM-capable GPIO for brightness control)
VCC     → 3V3
GND     → GND
```

### Software Setup

1. **Install Libraries** (if not already present):
   - The Waveshare LCD driver code is included in `LCD_Driver.cpp/h`
   - Font files are in `font*.cpp/fonts.h`
   - Paint library is in `GUI_Paint.cpp/h`

2. **Create Your Sketch**:
```cpp
#include "DEV_Config.h"
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "fonts.h"
#include "tft_compat.h"

// Paste your Lopaka-generated code here
// [BEGIN lopaka generated]
static const unsigned char PROGMEM image_icon_bits[] = {...};
void drawMyScreen(void) {
    tft.fillScreen(0x0000);
    tft.setTextColor(0xFFFF);
    tft.setTextSize(2);
    tft.drawString("Hello!", 50, 100);
    tft.drawBitmap(10, 10, image_icon_bits, 32, 32, 0xE33F);
}
// [END lopaka generated]

void setup() {
    Config_Init();
    LCD_Init();
    
    // Landscape orientation (280×240)
    // Use ROTATE_90 for landscape; try ROTATE_270 if mirrored
    Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_90, WHITE);
    
    drawMyScreen();
}

void loop() {
    // Your animation/update logic here
}
```

3. **Compile & Upload**:
   - Board: "XIAO_ESP32S3" (or your target board)
   - Upload speed: 921600
   - Flash Mode: QIO

## Using With Lopaka

1. **Design in Lopaka**: Create your UI at 280×240 (landscape) or 240×280 (portrait)

2. **Export Code**: Use Lopaka's "Generate Code" with TFT_eSPI output

3. **Paste Into Sketch**: Copy the entire `[BEGIN lopaka generated]` ... `[END lopaka generated]` block

4. **Call Your Function**: Just call `drawYourScreen()` in `setup()` or `loop()`

That's it! No manual porting required.

## Color Reference

Lopaka outputs 16-bit RGB565 hex colors. Common ones:

```cpp
0x0000  // Black
0xFFFF  // White
0xF800  // Red
0x07E0  // Green
0x001F  // Blue
0xFFE0  // Yellow
0x07FF  // Cyan
0xF81F  // Magenta
0x801   // Near-black (Lopaka's typical background)
0xE33F  // Bright magenta/purple (Lopaka's typical accent)
0x6870  // Medium gray
0x506C  // Darker gray
```

## Font Size Mapping

| TFT_eSPI `setTextSize()` | Waveshare Font | Char Height |
|--------------------------|----------------|-------------|
| `1`                      | `Font12`       | 12px        |
| `2`                      | `Font24`       | 24px        |
| `3`                      | `Font24`       | 24px        |

Lopaka typically uses size `1` (small) and `2` (large/headers).

## Known Limitations

### 1. Ellipse → Circle Approximation
Waveshare's library doesn't support true ellipses, so `drawEllipse(x, y, rx, ry, color)` draws a circle with radius `(rx + ry) / 2`. This works well for Lopaka's typical use case (small selection indicators), but perfect ellipses aren't possible.

### 2. No Anti-Aliasing
Both TFT_eSPI and the Waveshare library render aliased (hard-edged) graphics. This is expected on low-power embedded displays.

### 3. Text Width Measurement
`textWidth()` returns an **approximation** based on font character width. It's accurate enough for most layouts but not pixel-perfect like TFT_eSPI's font metrics.

### 4. Text Background
When using `setTextColor(fg)` with one argument, text is drawn with a "transparent" background that uses the current screen background color. This matches TFT_eSPI behavior but means text won't erase what's underneath it.

### 5. Rounded Rectangles
`drawRoundRect()` and `fillRoundRect()` are stubbed out as regular rectangles. Add proper rounded rectangle rendering if your design uses them.

## Display Orientation

The Waveshare 1.83" LCD is **physically 240×280** (portrait), but Lopaka designs are typically **280×240** (landscape).

Use `Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_90, WHITE)` to rotate the logical canvas:

- `ROTATE_0` = Portrait, 0° (240×280)
- `ROTATE_90` = Landscape, 90° CW (280×240) ← Most common
- `ROTATE_180` = Portrait, 180° (240×280)
- `ROTATE_270` = Landscape, 270° CW (280×240)

If your image appears mirrored, try swapping `ROTATE_90` ↔ `ROTATE_270`.

## Animation Support

Lopaka animations are exported as **multi-file frame sequences**. To animate:

1. Export each frame from Lopaka as separate `.txt` files
2. Paste all frame functions into your sketch
3. Call them sequentially in `loop()`:

```cpp
unsigned long lastFrame = 0;
int currentFrame = 0;
const int FRAME_COUNT = 5;

void loop() {
    if (millis() - lastFrame > 42) {  // ~24 FPS
        switch(currentFrame) {
            case 0: drawFrame0(); break;
            case 1: drawFrame1(); break;
            case 2: drawFrame2(); break;
            case 3: drawFrame3(); break;
            case 4: drawFrame4(); break;
        }
        currentFrame = (currentFrame + 1) % FRAME_COUNT;
        lastFrame = millis();
    }
}
```

For sprite-based animations (like the dumbbell example in `/Animations/`), the animation files contain embedded frame arrays—see `dumbbell_animation.txt` for the pattern.

## Project Structure

```
LCD_1in83/
├── LCD_1in83.ino          # Main sketch (paste Lopaka code here)
├── tft_compat.h           # ⭐ The compatibility shim
├── DEV_Config.h/.cpp      # SPI/GPIO configuration
├── LCD_Driver.h/.cpp      # NV3030B display driver
├── GUI_Paint.h/.cpp       # Graphics primitives
├── fonts.h                # Font structure definitions
├── font8.cpp              # 8px font
├── font12.cpp             # 12px font
├── font16.cpp             # 16px font
├── font20.cpp             # 20px font
├── font24.cpp             # 24px font
├── font24CN.cpp           # Chinese characters (if needed)
├── image.h/.cpp           # (Optional) embedded images
└── README.md              # This file
```

## Troubleshooting

### "Blank white screen"
- Check wiring (especially CS → D0, not D9)
- Verify `Paint_NewImage()` rotation matches your physical orientation
- Try `Paint_Clear(BLACK);` before drawing to rule out white-on-white

### "Mirrored or rotated image"
- Change `ROTATE_90` to `ROTATE_270` (or vice versa) in `Paint_NewImage()`
- Lopaka's coordinate system assumes landscape; rotate the display to match

### "Text is garbled"
- Ensure `setTextSize()` is called before `drawString()`
- Check that font files (`font*.cpp`) are included in the project

### "Colors look wrong"
- Lopaka outputs RGB565 hex values—these are correct as-is
- If colors are inverted, check `Paint_NewImage()`'s initial color parameter

### "Icons not showing"
- Verify `PROGMEM` keyword is present: `static const unsigned char PROGMEM image_bits[] = {...};`
- Check bitmap dimensions match the `drawBitmap()` call

## Performance Tips

1. **Minimize full-screen redraws**: Use `fillRect()` to update only changed regions
2. **Cache repeated bitmaps**: Draw once to a buffer if redrawing the same icon multiple times
3. **Use smaller fonts**: Font12 is faster than Font24
4. **Batch updates**: Call `LCD_SetWindow()` once and write multiple pixels

## Advanced: Backlight Control

The current wiring has backlight tied to 3V3 (always-on). For PWM brightness control:

1. **Rewire**: Connect LCD BL to a PWM-capable GPIO (e.g., D2/GPIO3)
2. **Add to setup()**:
```cpp
pinMode(2, OUTPUT);
analogWrite(2, 128);  // 50% brightness (0-255)
```
3. **Fade in loop()**:
```cpp
for (int i = 0; i <= 255; i++) {
    analogWrite(2, i);
    delay(5);
}
```

## Credits

- **Lopaka**: [lopaka.app](https://lopaka.app/) by @mireq
- **Waveshare LCD Driver**: [waveshare.com](https://www.waveshare.com/1.83inch-lcd-module.htm)
- **tft_compat.h**: Created for this project to bridge the two ecosystems

## License

This compatibility shim (`tft_compat.h`) is provided as-is for use in your projects. The underlying Waveshare driver code retains its original license.

---

**Questions?** Open an issue or check the Lopaka Discord for UI design help!
