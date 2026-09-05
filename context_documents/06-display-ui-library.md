# Display & UI Integration — TFT Compatibility Library

## Overview

The project uses **Lopaka** (visual screen designer) to design all UI screens, which generates code for the TFT_eSPI library. However, the hardware (Waveshare 1.83" LCD with NV3030B driver) requires a manufacturer-specific library that is incompatible with TFT_eSPI.

**Solution**: A drop-in compatibility shim (`tft_compat.h`) that translates TFT_eSPI calls to Waveshare's GUI_Paint/LCD_Driver API, allowing unmodified Lopaka-generated code to work.

## Directory Structure

```
Hardware/Display/
├── LCD_1.83_Code/              # Manufacturer's reference implementations
│   ├── ESP32/LCD_1in83/        # ESP32 base library (fonts, drivers, paint)
│   ├── Arduino/LCD_1inch83/    # Arduino variant
│   └── ...                     # Other platform variants
│
├── TFT_Compat_Library/         # ⭐ The compatibility layer
│   ├── tft_compat.h            # Main compatibility shim
│   ├── README.md               # Complete usage documentation
│   └── example_usage.ino       # Integration example
│
└── Lopaka Screens/             # UI designs exported from Lopaka
    ├── Animations/             # Animation frame sequences
    │   ├── dumbbell_animation.txt     # 48×48px @ specific location
    │   ├── back_to_home_animation.txt
    │   └── ...
    ├── Home Screen/            # Main screen variants
    ├── Menu/                   # Menu system screens
    ├── Log Workout/            # Workout logging screens
    ├── Settings/               # Settings screens
    ├── Music queue/            # Music control screens
    └── Top Bar/                # Status bar component
```

## TFT Compatibility Library

### Purpose
Bridges the API gap between:
- **Lopaka's target**: TFT_eSPI library (popular, well-documented, many tools support it)
- **Hardware reality**: Waveshare GUI_Paint/LCD_Driver (vendor-specific, required for NV3030B chip)

### What It Does
Provides a `tft` global object that implements the full TFT_eSPI API surface used by Lopaka:

**Drawing Primitives:**
- `fillScreen(color)` — clear to solid color
- `drawRect(x, y, w, h, color)` / `fillRect(...)` — rectangles
- `drawLine(x0, y0, x1, y1, color)` — lines/dividers
- `drawEllipse(x, y, rx, ry, color)` — selection indicators (approximated as circles)
- `drawCircle(x, y, r, color)` / `fillCircle(...)`
- `drawPixel(x, y, color)` — individual pixels

**Text Rendering:**
- `setTextColor(fg)` / `setTextColor(fg, bg)` — text colors
- `setTextSize(1|2|3)` — maps to Waveshare fonts (1→Font12, 2→Font24)
- `drawString(text, x, y)` — render text
- `setFreeFont()` — stub for compatibility

**Image Rendering:**
- `drawBitmap(x, y, data, w, h, color)` — monochrome 1bpp images (Lopaka's icon format)

### Integration Pattern

```cpp
#include "DEV_Config.h"
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "fonts.h"
#include "tft_compat.h"  // ← The compatibility layer

// Paste Lopaka code here unchanged
// [BEGIN lopaka generated]
void drawHomeScreen(void) {
    tft.fillScreen(0x0801);
    tft.setTextColor(0xE33F);
    tft.setTextSize(2);
    tft.drawString("Gym Assistant", 50, 40);
}
// [END lopaka generated]

void setup() {
    Config_Init();
    LCD_Init();
    Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_90, WHITE);
    drawHomeScreen();  // Just call it — no porting needed
}
```

### Key Mapping Details

**Colors**: 16-bit RGB565 hex (Lopaka's output format)
- `0x0801` — Near-black (typical background)
- `0xE33F` — Bright magenta (primary accent in designs)
- `0x6870` — Medium gray (borders, dividers)
- `0x506C` — Dark gray (inactive text)
- `0xFFFF` — White (primary text)

**Fonts**: Text size → Waveshare font mapping
- `setTextSize(1)` → `Font12` (12px, body text)
- `setTextSize(2)` → `Font24` (24px, headers/large text)
- `setTextSize(3)` → `Font24` (same as 2, no larger font available)

**Coordinate System**: Landscape 280×240
- Physical panel: 240×280 (portrait)
- Logical canvas: 280×240 (rotated 90° with `ROTATE_90`)
- All Lopaka designs assume landscape orientation

### Known Limitations

1. **Ellipses → Circles**: Waveshare has no native ellipse support, so `drawEllipse(x, y, rx, ry, color)` draws a circle with radius `(rx+ry)/2`. Works well for small selection indicators but not perfect ellipses.

2. **Text Width Approximation**: `textWidth()` estimates character width based on font, not true glyph metrics. Accurate enough for typical layouts.

3. **No Anti-Aliasing**: Both libraries render aliased (hard-edged) graphics — expected on embedded displays.

4. **Rounded Rectangles**: Stubbed as regular rectangles (Lopaka rarely uses them).

## Lopaka Screen Assets

### Screen Inventory

All screens designed at 280×240 landscape in Lopaka's visual editor. Each exported as:
- `.txt` file — C++ code with draw function + embedded PROGMEM bitmaps
- `.png` file — Visual reference of the design

**Screen Categories:**

1. **Home Screen** (4 variants)
   - `Home_screen_both_1.txt` — Workout + Music visible
   - `Home_screen_both_2.txt` — Alternative layout
   - `Home_screen_music_only.txt` — Music-only (no active workout)
   - `Home_screen_workout_only.txt` — Workout-only (phone disconnected)

2. **Menu** (3 screens)
   - `menu_1.txt` — "Workout Logs" selected
   - `menu_2.txt` — "Music Queue" selected
   - `menu_3.txt` — "Settings" selected

3. **Log Workout** (5 screens)
   - Workout selection, exercise list, set logging, stats view

4. **Settings** (4 screens)
   - Main settings menu, display settings, hype/rest config, brightness

5. **Music Queue** (1 screen)
   - Shuffle/repeat controls + upcoming tracks

6. **Top Bar** (reusable component)
   - Time, date, Bluetooth status, battery level

### Animation Assets

Located in `Lopaka Screens/Animations/` — these are **specific-sized placeholders** for animations that will be inserted at marked locations in the final screens.

**Animation Inventory:**
- `dumbbell_animation.txt` — 48×48px spinning dumbbell (menu item icon)
- `bench_press_animation.txt` — Exercise-specific icon
- `deadlift_animation.txt` — Exercise-specific icon
- `back_to_home_animation.txt` — Navigation transition
- Various settings animations (display, brightness, pairing, etc.)

**Animation Format**: Multi-frame sprite sequences
- Each frame is a PROGMEM byte array
- Frame dimensions specified in filename or code comment
- Render by cycling through frames in `loop()` at ~24 FPS

**Integration Pattern**:
```cpp
const unsigned char PROGMEM frame0[] = {...};
const unsigned char PROGMEM frame1[] = {...};
// ... more frames

int currentFrame = 0;
unsigned long lastFrame = 0;

void loop() {
    if (millis() - lastFrame > 42) {  // ~24 FPS
        tft.drawBitmap(x, y, frames[currentFrame], 48, 48, 0xE33F);
        currentFrame = (currentFrame + 1) % FRAME_COUNT;
        lastFrame = millis();
    }
}
```

### Asset Files (PNG images)

Each screen folder contains supporting PNG assets:
- **Icons**: Bluetooth, battery, music, settings, back button, info button
- **Selection indicators**: Rounded rectangles, pointer arrows
- **Dividers**: Lines, margins, separator bars
- **UI elements**: Tick boxes, sliders, volume controls

These are **reference images** — the actual code uses the embedded PROGMEM bitmaps in the `.txt` files, which are already converted to the 1bpp monochrome format the display requires.

## Display Hardware Specifications

**Panel**: Waveshare 1.83" IPS LCD
- Resolution: 240×280 (portrait physical, 280×240 landscape logical)
- Driver: NV3030B
- Interface: SPI
- Colors: 65K (16-bit RGB565)
- Refresh: ~30-60 FPS depending on content

**Pin Mapping** (XIAO ESP32S3):
```
LCD RST → D3 (GPIO4)
LCD DC  → D1 (GPIO2)
LCD CS  → D0 (GPIO1)  ⚠️ NOT D9 (MISO conflict)
LCD DIN → D10 (GPIO9) (SPI MOSI)
LCD CLK → D8 (GPIO7)  (SPI SCK)
LCD BL  → 3V3 (always-on; move to PWM GPIO for brightness control)
```

**Orientation**:
- `ROTATE_0` — Portrait 0° (240×280)
- `ROTATE_90` — Landscape 90° CW (280×240) ← **Lopaka target**
- `ROTATE_180` — Portrait 180° (240×280)
- `ROTATE_270` — Landscape 270° CW (280×240)

Set via `Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_90, WHITE);` in `setup()`.

## Firmware Integration Points

### Phase 1 — Firmware Skeleton (see `02-firmware.md`)

**Display Task** responsibilities:
1. Own the SPI bus and LCD driver exclusively
2. Maintain a screen state machine (which screen is active)
3. Render screens via Lopaka-generated draw functions
4. Handle animated selection icons (sprite-frame loop)
5. Respond to render events from app logic task

**Key Design Decision** (from firmware doc):
> Selected menu items show a subtle sprite-frame animation (e.g., spinning dumbbell icon); unselected items render as static icons. This must be part of the render loop design from the start, not bolted on later.

**Implementation Pattern**:
```cpp
// Display task pseudocode
typedef enum {
    SCREEN_HOME_BOTH,
    SCREEN_HOME_MUSIC,
    SCREEN_MENU,
    SCREEN_WORKOUT_LOG,
    // ... more screens
} screen_id_t;

screen_id_t current_screen = SCREEN_HOME_BOTH;
int selected_item = 0;
int anim_frame = 0;

void display_task(void* params) {
    while (1) {
        // Check for screen change events from app logic task
        if (xQueueReceive(screen_event_queue, &new_screen, 0)) {
            current_screen = new_screen;
            redraw_full_screen();
        }
        
        // Animate selected item (every ~42ms for 24 FPS)
        if (should_update_animation()) {
            update_selection_animation();
            anim_frame = (anim_frame + 1) % FRAME_COUNT;
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));  // 100Hz tick rate
    }
}
```

### UI State Management

**Screen State** lives in the Display Task:
- Current screen ID
- Selected item index (for menus/lists)
- Animation frame counter
- Dynamic content (time, battery, BLE status for top bar)

**Input Events** come from Input Task via queue:
- `ENCODER_CW` / `ENCODER_CCW` — scroll selection
- `ENCODER_PRESS` — confirm/select
- `BUTTON_HYPE` / `BUTTON_REST` — context-dependent (see below)

**Navigation Logic** lives in App Logic Task:
- Interprets input events based on current screen
- Posts screen change events to Display Task
- Manages deep navigation state (menu → submenu → setting)

## Cross-Reference to Other Docs

- **Hardware**: See `01-device-hardware.md` for pin mapping, display specs
- **Firmware Architecture**: See `02-firmware.md` for FreeRTOS task structure, display task design
- **UI/UX Design**: See `04-ui-ux.md` for design system (colors, fonts, layout), screen inventory, interaction patterns
- **Progress**: See `05-progress.md` for Phase 1 Lopaka screen porting checklist

## Development Workflow

1. **Design in Lopaka**: Create UI at 280×240 landscape
2. **Export Code**: Generate TFT_eSPI code
3. **Paste into Firmware**: Copy entire `[BEGIN lopaka generated]` block
4. **Include tft_compat.h**: Single `#include` makes it work
5. **Call from Display Task**: `drawYourScreen()` renders immediately

No manual porting, no API translation — the compatibility layer handles everything.

## Future Considerations

### Backlight PWM Control
Current wiring: BL → 3V3 (always full brightness)

For power saving (Phase 5):
1. Rewire BL to PWM-capable GPIO (e.g., D2/GPIO3)
2. Add to DEV_Config: `pinMode(2, OUTPUT); analogWrite(2, brightness);`
3. Dim/sleep logic in Display Task based on Lock state and idle timeout

### Screen Dim/Sleep (Phase 5)
- Lock state → backlight off, display frozen
- Idle timeout → backlight dim to 25%, full black screen
- Any input → restore full brightness + last screen

### OTA Updates (Phase 5+)
- Potential: Push new Lopaka screens over BLE
- Screens stored in SPIFFS/LittleFS as compiled bytecode
- Display task loads screen by ID from filesystem
- Enables UI updates without firmware reflash

---

**Last Updated**: 2026-09-04  
**Status**: TFT compatibility layer complete, ready for Phase 1 integration
