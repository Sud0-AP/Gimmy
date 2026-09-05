# Lopaka UI Screens — Organization & Structure for AI Discussion

This document explains how the Lopaka-designed UI screens are organized, how to read their code and assets, and what AI agents need to know to discuss the next implementation steps for making the interactive menu/UI work with dummy data.

---

## What Lopaka Is and Why We Use It

**Lopaka** is a visual screen designer tool for embedded displays that exports code targeting the `TFT_eSPI` graphics library. We use it to design all UI screens for the Gym Assistant hardware device.

**The Problem**: The Waveshare 1.83" LCD uses a proprietary driver (`GUI_Paint`/`LCD_Driver`) incompatible with `TFT_eSPI`.

**The Solution**: A drop-in compatibility shim (`Hardware/Display/TFT_Compat_Library/tft_compat.h`) that translates all `TFT_eSPI` calls to Waveshare's API, allowing unmodified Lopaka code to render pixel-perfectly on the hardware.

---

## Directory Structure & File Organization

```
Hardware/Display/Lopaka Screens/
├── Home Screen/
│   ├── Home_screen_both_1.txt           # Code: Workout + Music visible
│   ├── Home_screen_both_1.png           # Visual reference
│   ├── Home_screen_both_2.txt
│   ├── Home_screen_both_2.png
│   ├── Home_screen_music_only.txt
│   ├── Home_screen_music_only.png
│   ├── Home_screen_workout_only.txt
│   └── Home_screen_workout_only.png
│
├── Menu/
│   ├── menu_1.txt                       # Code: "Workout Logs" selected
│   ├── menu_1.png                       # Visual reference
│   ├── menu_2.txt                       # Code: "Music Queue" selected
│   ├── menu_2.png
│   ├── menu_3.txt                       # Code: "Settings" selected
│   └── menu_3.png
│
├── Log Workout/
│   ├── log_workout_1.txt                # Choose Split screen
│   ├── log_workout_1.png
│   ├── log_workout_2.txt                # Choose Workout screen
│   ├── log_workout_2.png
│   ├── log_workout_3.txt                # Workout Overview
│   ├── log_workout_3.png
│   ├── log_workout_4.txt                # Active Logging Session
│   ├── log_workout_4.png
│   ├── log_workout_5.txt                # Exercise Stats / PR view
│   └── log_workout_5.png
│
├── Settings/
│   ├── Settings_main.txt                # Main settings menu
│   ├── Settings_main.png
│   ├── settings_display.txt             # Display settings submenu
│   ├── settings_display.png
│   ├── settings_hype_rest.txt           # Hype/Rest timer config
│   ├── settings_hype_rest.png
│   ├── settings_brightness.txt          # Brightness slider
│   └── settings_brightness.png
│
├── Music queue/
│   ├── music_queue.txt                  # Shuffle/repeat + track list
│   └── music_queue.png
│
├── Top Bar/
│   ├── top_bar.txt                      # Reusable status bar component
│   └── top_bar.png
│
├── Animations/
│   ├── dumbbell_animation.txt           # 48×48px spinning dumbbell
│   ├── bench_press_animation.txt        # 64×64px exercise icon
│   ├── deadlift_animation.txt           # 64×64px exercise icon
│   ├── back_to_home_animation.txt       # 32×32px nav transition
│   ├── settings_animation.txt           # 48×48px settings gear
│   └── ... (more animation sprites)
│
└── Assets/                               # PNG reference images
    ├── Icons/
    │   ├── bluetooth.png
    │   ├── battery.png
    │   ├── back_button.png
    │   ├── info_button.png
    │   └── ...
    ├── Selection/
    │   ├── rounded_pill.png
    │   ├── pointer_arrow.png
    │   └── ...
    └── UI Elements/
        ├── divider.png
        ├── checkbox.png
        └── ...
```

---

## What's Inside Each `.txt` File

Every `.txt` file contains **complete C++ drawing code** structured as:

1. **PROGMEM Bitmap Arrays**: 1-bit-per-pixel monochrome icon/image data
   ```cpp
   static const unsigned char PROGMEM image_Layer_28_bits[] = {
       0xff,0xff,0xff,0xff,0xff,0xff,0x01,0xc0,0x01,0xff,
       // ... byte array continues
   };
   ```

2. **Drawing Function**: A `void` function that renders the entire screen
   ```cpp
   void draw(void) {
       tft.fillScreen(0x0801);  // Background color
       
       // Draw UI elements
       tft.setTextSize(2);
       tft.setTextColor(0xE33F);
       tft.drawString("Workout Logs", 75, 52);
       
       // Draw bitmaps (icons, selection pills, etc.)
       tft.drawBitmap(12, 25, image_Layer_28_bits, 240, 36, 0xE33F);
       
       // ... more drawing calls
   }
   ```

3. **Color Codes**: 16-bit RGB565 hex values
   - `0x0801` — Near-black background
   - `0xE33F` — Bright magenta accent (primary selection color)
   - `0xFFFF` — White (primary text)
   - `0x6870` — Medium gray (borders, dividers)
   - `0x506C` — Dark gray (inactive text/borders)

4. **Key Variables**: Selection state indicators (when present)
   ```cpp
   int back_selection_color = 0x801;   // Back button: unselected (invisible)
   int Info_selection_color = 0x801;   // Info button: unselected (invisible)
   // When selected, these become 0xE33F
   ```

---

## What's Inside Each `.png` File

The `.png` files are **visual reference images** showing exactly what the screen looks like when rendered. They are:
- Screenshots of the Lopaka design canvas
- 280×240 pixels (landscape orientation)
- Exported alongside the code for human reference

**Important**: The PNG files are NOT used by the firmware. The actual graphics come from the PROGMEM bitmap arrays embedded in the `.txt` code files. The PNGs exist purely so humans (and AI agents) can see what the screen should look like without running the code.

---

## How UI Selection & Navigation Works

The UI is **fully static code** exported from Lopaka — there are no "dynamic elements" baked into the `.txt` files. Navigation works by:

### 1. **Multiple Screens per Menu State**
Instead of one dynamic screen, we have separate static screens for each selection state:
- `menu_1.txt` — Main Menu with "Workout Logs" selected (selection pill at `y=25`)
- `menu_2.txt` — Main Menu with "Music Queue" selected (selection pill at `y=97`)
- `menu_3.txt` — Main Menu with "Settings" selected (selection pill at `y=169`)

The firmware's display task renders the correct `.txt` file based on `selectedIndex`.

### 2. **Color Swapping for Selection** (Pattern A)
Unselected items:
- Thin border: `0x506C` (dark gray)
- Text: `0x506C` or `0x6870` (gray)
- Icon: Static frame 0

Selected items:
- Thick pill/outline: `0xE33F` (magenta) or `0xFFFF` (white)
- Text: `0xE33F` or `0xFFFF`
- Icon: Animated sprite loop (24 FPS)

### 3. **Selection Dot for Header Buttons** (Pattern B)
Back and Info buttons use an ellipse indicator:
```cpp
int back_selection_color = 0x801;  // Unselected = background color (invisible)
// When selected: back_selection_color = 0xE33F (visible magenta dot)
tft.drawEllipse(13, 10, 8, 6, back_selection_color);
```

### 4. **Scrollbar Thumb Position** (Pattern C)
Scrollbar thumb Y-coordinate shifts based on selected item:
```cpp
// Menu Item 0: thumb_y = 59
// Menu Item 1: thumb_y = 106
// Menu Item 2: thumb_y = 153
int thumb_y = 59 + (selectedIndex * 47);
tft.fillRect(265, thumb_y, 9, 15, 0xE33F);
```

### 5. **Set Marker in Workout Logging** (Pattern D)
Active set marker shifts vertically through the set list:
```cpp
int current_set_marker_y = 140;  // Default: Set 3
// Set 1: y=113, Set 2: y=126, Set 3: y=140, Set 4: y=154, Set 5: y=168
tft.drawRect(148, current_set_marker_y, 124, 13, 0x6870);
```

---

## Animation Slots & Sprite Placement

### What Animations Are
Animations are **multi-frame sprite sequences** stored as separate PROGMEM bitmap arrays in `Animations/*.txt` files.

Example from `dumbbell_animation.txt`:
```cpp
const unsigned char PROGMEM frame_0[] = { /* 48×48px data */ };
const unsigned char PROGMEM frame_1[] = { /* 48×48px data */ };
const unsigned char PROGMEM frame_2[] = { /* 48×48px data */ };
// ... up to frame_N
```

### Where They Go
In Lopaka screens, **square bounding boxes** mark animation slots:
- Main Menu icon slots: `48×48` @ `(8, 36)`, `(8, 108)`, `(8, 181)`
- Split/Workout select icon: `64×64` @ `(13, 105)`
- Home screen active exercise: `32×32` @ `(151, 35)`

### Rendering Logic
```cpp
// Unselected item: draw frame 0 (static)
tft.drawBitmap(8, 36, frame_0, 48, 48, 0xE33F);

// Selected item: loop through frames at ~24 FPS
int currentFrame = 0;
unsigned long lastFrameTime = 0;

void loop() {
    if (millis() - lastFrameTime > 42) {  // 42ms = ~24 FPS
        tft.drawBitmap(8, 36, frames[currentFrame], 48, 48, 0xE33F);
        currentFrame = (currentFrame + 1) % TOTAL_FRAMES;
        lastFrameTime = millis();
    }
}
```

---

## How to Convert Lopaka Screens into Working Firmware

### Current State: Static Lopaka Exports
Each `.txt` file is a **snapshot** of one specific UI state. They are designed to be **pasted directly into Arduino/ESP-IDF sketches** with `#include "tft_compat.h"`.

### Next Step: Parameterized Drawing Functions
To make the UI interactive, convert static Lopaka code into **state-driven drawing functions**:

#### Example: Main Menu Conversion
**Before** (Lopaka static export):
```cpp
// menu_1.txt — "Workout Logs" selected
void draw(void) {
    tft.fillScreen(0x0801);
    tft.drawBitmap(12, 25, selection_pill_bits, 240, 36, 0xE33F);  // Fixed Y=25
    tft.setTextColor(0xE33F);
    tft.drawString("Workout Logs", 75, 52);
    // ... more fixed drawing
}
```

**After** (Parameterized for firmware):
```cpp
void drawMainMenu(uint8_t selectedIndex, uint8_t animFrame) {
    tft.fillScreen(0x0801);
    
    // Draw top bar
    drawTopBar("12:23 PM", "14th May", true, 85);
    
    // Draw menu items with conditional styling
    const char* items[] = {"Workout Logs", "Music Queue", "Settings"};
    int baseY[] = {52, 125, 198};
    
    for (int i = 0; i < 3; i++) {
        uint16_t textColor = (i == selectedIndex) ? 0xE33F : 0x506C;
        tft.setTextColor(textColor);
        tft.drawString(items[i], 75, baseY[i]);
        
        if (i == selectedIndex) {
            // Draw selection pill at dynamic Y position
            int pillY = 25 + (i * 72);
            tft.drawBitmap(12, pillY, selection_pill_bits, 240, 36, 0xE33F);
            
            // Draw animated icon at dynamic Y position
            int iconY = 36 + (i * 72);
            tft.drawBitmap(8, iconY, animFrames[i][animFrame], 48, 48, 0xE33F);
        } else {
            // Draw static icon (frame 0 only)
            int iconY = 36 + (i * 72);
            tft.drawBitmap(8, iconY, animFrames[i][0], 48, 48, 0x6870);
        }
    }
    
    // Draw scrollbar thumb at dynamic Y
    int thumbY = 59 + (selectedIndex * 47);
    tft.fillRect(265, thumbY, 9, 15, 0xE33F);
}
```

### State Machine Structure
```cpp
typedef enum {
    SCREEN_HOME_BOTH,
    SCREEN_HOME_MUSIC,
    SCREEN_HOME_WORKOUT,
    SCREEN_MENU,
    SCREEN_LOG_WORKOUT_CHOOSE_SPLIT,
    SCREEN_LOG_WORKOUT_CHOOSE_WORKOUT,
    SCREEN_SETTINGS_MAIN,
    // ... more screens
} screen_id_t;

struct UIState {
    screen_id_t currentScreen;
    uint8_t selectedIndex;
    uint8_t animFrame;
    bool isDirty;  // Requires full redraw
};

UIState uiState = {
    .currentScreen = SCREEN_HOME_BOTH,
    .selectedIndex = 0,
    .animFrame = 0,
    .isDirty = true
};
```

---

## What AI Agents Need to Know for Next Steps

### 1. **The Goal**: Interactive UI with Dummy Data
We want to build a working menu navigation system that:
- Responds to rotary encoder input (CW/CCW to scroll, PRESS to select)
- Renders the correct screen based on navigation state
- Animates selected items at 24 FPS
- Uses **dummy hardcoded data** (no real Spotify API, no real workout templates)

### 2. **The Current State**
✅ **Complete**:
- All UI screens designed and exported from Lopaka
- `tft_compat.h` compatibility layer tested and working
- Hardware pinout confirmed (SPI display, encoder on native GPIO)
- Design system documented (colors, fonts, selection patterns)

🚧 **Next Phase** (Phase 1 — Firmware Skeleton):
- Convert Lopaka static exports into parameterized drawing functions
- Build screen state machine (see `context_documents/07-ui-interaction-spec.md`)
- Implement encoder input handling (interrupt-driven, debounced)
- Create display task (FreeRTOS) with 24 FPS animation loop
- Wire up navigation: Encoder CW/CCW → update `selectedIndex`, Encoder PRESS → change screen

### 3. **Key Design Decisions to Discuss**
When planning the next implementation steps with an AI agent, the important questions are:

**A. Screen State Management**:
- Should each screen be a separate function (`drawMainMenu()`, `drawChooseSplit()`, etc.) or a single `drawScreen(screen_id, state)` dispatcher?
- How to handle nested menus (Main Menu → Settings → Display Settings → Brightness)?
- Should navigation history be a stack (Back button = pop) or explicit parent pointers?

**B. Animation Architecture**:
- Where do animation frame arrays live? (Global arrays? Separate header files? LittleFS?)
- How to map screen + selectedIndex → correct animation slot?
- Should unselected items render frame 0 once, or re-render every loop?

**C. Dummy Data Structure**:
- How to represent dummy workout templates, splits, exercises?
- Should dummy music queue be a hardcoded array or JSON in flash?
- Where does dummy battery/BLE/time data come from? (Hardcoded constants? Simulated ticker?)

**D. Input Handling**:
- Should encoder events be queued (FreeRTOS queue) or polled (shared volatile variable)?
- How to debounce encoder rotation? (Timer-based? Interrupt count threshold?)
- Should button context (Hype/Rest vs Back/Save) live in input task or app logic task?

**E. Code Organization**:
- One monolithic `main.cpp` with all drawing functions, or split into `screens/`, `ui/`, `input/`, `app_logic/`?
- Should Lopaka code stay in separate `.h` files (`menu_1.h`, `menu_2.h`) or be merged into parameterized functions?

### 4. **What NOT to Do**
❌ **Don't try to "parse" or "interpret" Lopaka code dynamically** — the `.txt` files are meant to be copy-pasted into firmware source code, not loaded at runtime.

❌ **Don't assume the PNG images are used by firmware** — they are reference-only for humans.

❌ **Don't plan to modify the compatibility library** — `tft_compat.h` is complete and tested; all future work happens at the application layer.

---

## Example Discussion Prompt for Claude Web

When uploading this document to the Claude web interface, you can ask questions like:

> "I've designed all the UI screens for my embedded gym device in Lopaka (see the attached context). Each screen is a static snapshot, but I need to make them interactive with a rotary encoder. What's the best way to convert these static screens into a state-driven menu system with 24 FPS selection animations? Should I use one drawing function per screen or a unified dispatcher? How should I structure the state machine and animation frame management?"

Or:

> "Looking at my Lopaka screen organization (context attached), I want to build Phase 1 of my firmware: a working menu with dummy data. Can you help me design the state machine architecture, the FreeRTOS task breakdown, and the file structure for converting the static Lopaka code into parameterized drawing functions?"

---

## Cross-References

- **Hardware Specs**: See `context_documents/01-device-hardware.md`
- **Firmware Architecture**: See `context_documents/02-firmware.md`
- **UI Design System**: See `context_documents/04-ui-ux.md`
- **Display Library Details**: See `context_documents/06-display-ui-library.md`
- **Navigation Patterns**: See `context_documents/07-ui-interaction-spec.md`
- **Compatibility Library**: See `Hardware/Display/TFT_Compat_Library/README.md`

---

**Last Updated**: 2026-09-04  
**Status**: Ready for Phase 1 firmware implementation planning
