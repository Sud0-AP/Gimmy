# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**Gym Assistant** is a standalone, pocketable hardware companion device and mobile app (Android/iOS) for distraction-free gym workouts. It serves two primary functions:
1. Tactile remote control for Spotify music playback (with custom Hype/Rest playlist timers).
2. Guided, offline-first workout logging (sets, reps, weights, exercise stats) using a rotary encoder.

The device operates standalone with on-device flash caching (source of truth for workout data). The mobile companion app handles Spotify OAuth, Spotify Web API calls, and the full workout database.

---

## Repository Structure

```
Gym Assistant/
├── context_documents/             # Core architectural and specification docs
│   ├── 00-project-overview.md     # Vision, BOM summary, phase roadmap
│   ├── 01-device-hardware.md      # Confirmed pinout, battery/power specs, BOM
│   ├── 01-ble-communication.md    # GATT roles, sync protocol, OS background limits
│   ├── 02-firmware.md             # FreeRTOS task breakdown, state machines
│   ├── 02-music-control.md        # Spotify playback, Hype/Rest snapshot mechanics
│   ├── 03-workout-logging.md      # Template sync, guided workout flow, stats
│   ├── 04-ui-ux.md                # Design system, screen inventory, interaction rules
│   ├── 05-progress.md             # Milestone checklist and status tracking
│   ├── 06-display-ui-library.md   # Waveshare LCD driver & Lopaka shim architecture
│   ├── 07-ui-interaction-spec.md  # Visual patterns + animation rules + per-screen behavior (encoder/button, entry/exit) — gated spec
│   └── 08-lopaka-screens-explained.md # How to read Lopaka .txt/.png exports; static→parametric conversion
├── Hardware/
│   ├── Display/
│   │   ├── LCD_1.83_Code/         # Waveshare vendor reference SDKs (ESP32, Arduino, Pico, STM32)
│   │   ├── Lopaka Screens/        # Exported UI screens (.txt code, .png previews, Animations/)
│   │   └── TFT_Compat_Library/    # Drop-in TFT_eSPI compatibility shim (tft_compat.h)
│   └── Unit_Tests/
│       ├── Display_Encoder_Test/  # Breadboard verification sketch for display + encoder
│       └── Lopaka_Screen_Test/    # UI render validation sketch
```

---

## Hardware Architecture & Confirmed Pinout

- **MCU**: Seeed Studio XIAO ESP32S3 (ESP-IDF native target, FreeRTOS tasks).
- **Display**: Waveshare 1.83" IPS LCD (240×280 physical portrait, NV3030B controller, operated in 280×240 landscape via `ROTATE_90`).
- **Encoder**: M274 360° rotary encoder on **native GPIO** with interrupts (single `CHANGE` interrupt on CLK).
- **I/O Expander**: PCF8574T I2C 8-bit expander for 5 push buttons, encoder push button, and 3-way switch.
- **Power Switch**: 2P2T (DPDT) ON-OFF-ON switch (Off = hardware cut, On = normal, Lock = screen dark, input ignored, timers keep running).

### XIAO ESP32S3 Pin Mapping

**IMPORTANT: Encoder and I2C share pins D4/D5 — only encoder is currently wired.**

| Signal | XIAO Pin | ESP32 GPIO | Notes |
|---|---|---|---|
| LCD RST | D3 | GPIO4 | Reset |
| LCD DC | D1 | GPIO2 | Data / Command |
| LCD CS | D0 | GPIO1 | **Must be D0** (D9 is default MISO, conflicts with SPI) |
| LCD DIN | D10 | GPIO9 | Hardware SPI MOSI |
| LCD CLK | D8 | GPIO7 | Hardware SPI SCK |
| LCD BL | 3V3 | — | Backlight tied high (route to PWM GPIO for dimming in Phase 5) |
| Encoder CLK | D6 | GPIO43 | Native interrupt pin (CLK channel) — **moved from D4** |
| Encoder DT | D7 | GPIO44 | Native GPIO read (direction decode) — **moved from D5** |
| I2C SDA | D4 | GPIO5 | PCF8574T expander — shares hardware with relocated encoder |
| I2C SCL | D5 | GPIO6 | PCF8574T expander — shares hardware with relocated encoder |

**PCF8574T I2C Expander** (confirmed working, address 0x20):

| Function | Expander Pin |
|---|---|
| Previous | P2 |
| Play/Pause | P4 |
| Next | P0 |
| Hype | P3 |
| Rest | P1 |
| (Remaining) | P5–P7 (encoder push-button + 3-way switch, not yet assigned) |

---

## Display & UI Subsystem

### Lopaka to Hardware Pipeline
1. UI screens are designed in Lopaka (280×240 landscape) targeting the `TFT_eSPI` API.
2. `Hardware/Display/TFT_Compat_Library/tft_compat.h` provides a global `tft` instance that maps `TFT_eSPI` calls directly to Waveshare's `GUI_Paint`/`LCD_Driver` primitives.
3. Generated Lopaka code blocks (`[BEGIN lopaka generated]` ... `[END lopaka generated]`) can be pasted directly into sketches without manual rewriting.

### UI Design Tokens & Conventions
- **Background**: `0x0801` (near-black).
- **Primary Accent**: `0xE33F` (bright magenta/purple) for active selections, progress bars, highlights.
- **Primary Text**: `0xFFFF` (white).
- **Secondary Text / Inactive Borders**: `0x6870` / `0x506C` (gray shades).
- **Font Sizing**: `setTextSize(1)` maps to `Font12` (12px), `setTextSize(2)` maps to `Font24` (24px).
- **Selection States**:
  - *Lists/Pills*: Unselected = `0x506C`/`0x6870` thin outline; Selected = `0xE33F` thick pill.
  - *Header Buttons (Back/Info)*: Unselected = `tft.drawEllipse()` in background color `0x0801` (invisible); Selected = `0xE33F`.
  - *Scrollbars*: Vertical thumb Y-position is dynamically calculated: `thumb_y = base_y + (index * step_y)`.
- **Animations**: Bounding placeholders in Lopaka screens (32×32, 48×48, 64×64) render frame 0 statically when idle, and loop sprite arrays from `Hardware/Display/Lopaka Screens/Animations/` at ~24 FPS (42ms partial redraw) when selected.

---

## Development & Build Workflows

### Arduino Bring-up Sketches (Current Phase 0)
The bring-up tests are located in `Hardware/Unit_Tests/`:
- `Hardware/Unit_Tests/Lopaka_Screen_Test/LCD_1in83/`
- `Hardware/Unit_Tests/Display_Encoder_Test/LCD_1in83/`

**Arduino CLI Commands**:
```bash
# Compile bring-up sketch for XIAO ESP32S3
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32S3 "Hardware/Unit_Tests/Lopaka_Screen_Test/LCD_1in83"

# Upload sketch over USB
arduino-cli upload -p /dev/ttyACM0 --fqbn esp32:esp32:XIAO_ESP32S3 "Hardware/Unit_Tests/Lopaka_Screen_Test/LCD_1in83"
```

### ESP-IDF Native Firmware (Target Architecture — Phase 1+)
The production firmware will use **native ESP-IDF with FreeRTOS** tasks:
- `display_task`: Owns SPI bus, runs render loop and 24 FPS selection animations.
- `input_task`: Polls PCF8574T over I2C and handles encoder native GPIO interrupts.
- `ble_task`: Manages GATT server and phone communication.
- `app_logic_task`: Owns screen state machine, timers, and workout session state.
- `storage_task`: Manages NVS/LittleFS flash cache for 2–3 recent workout templates and settings.

**ESP-IDF Build Commands** (when firmware directory is scaffolded):
```bash
idf.py set-target esp32s3
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

**Current status**: Phase 0a (hardware bring-up) — display, encoder, PCF8574T, and buttons all confirmed working on breadboard. 3-way switch wiring and Phase 0b power validation are pending. No ESP-IDF firmware directory exists yet; all code so far lives in the Arduino bring-up sketches. See `context_documents/05-progress.md` for the live checklist before starting new work.

---

## Critical Gotchas (learned the hard way — see 05-progress.md)

- **`Paint_NewImage` argument order**: Pass the *physical* panel dimensions (240, 280) as Width/Height regardless of rotation — NOT the rotated canvas size. Passing rotated dimensions corrupts `Paint_Clear` addressing and scrambles rendering. Correct: `Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_90, WHITE)`.
- **LCD CS must be D0, not D9**: D9 is the XIAO's default hardware MISO pin; sharing it with CS breaks the SPI peripheral even though this display never uses MISO.
- **Encoder must stay on native GPIO**: Never move A/B to the PCF8574T. I2C polling latency drops/double-counts quadrature edges. Buttons on the expander are fine (simple debounced reads); the encoder is not.
- **If display renders mirrored**: swap `ROTATE_90` ↔ `ROTATE_270` in `Paint_NewImage()`.
- **`tft_compat.h` is complete — do not modify it**: All UI work happens at the application layer. Ellipses approximate as circles, rounded rects render as plain rects, and single-arg `setTextColor()` uses transparent (non-erasing) background — these are known, accepted limitations.

---

## Key Design Principles

1. **Device state survives disconnection**: All workout logging and timers execute fully offline; BLE drops must not freeze or interrupt workouts.
2. **Screen-based button context**: Hype/Rest buttons trigger timers on Home/Workout screens; on any menu/list/settings screen they are repurposed as fixed physical-button actions — **Hype (left) = Back, Rest (right) = Info** (context switch by active screen, no long-press gestures). These are deliberately NOT encoder-reachable. See `context_documents/07-ui-interaction-spec.md` for the locked per-screen behavior.
3. **No on-device text entry**: All templates, split names, and exercise libraries are authored in the mobile app; the device only selects and adjusts numeric values via the encoder.
4. **Display calls behind abstraction**: Always render through `tft_compat.h` or structured `draw_screen(screen_id, state)` functions to keep business logic isolated from hardware drivers.
