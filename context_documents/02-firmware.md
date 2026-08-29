# Hardware — Firmware

## Platform decision (locked)

**ESP-IDF native, not Arduino framework.** The Arduino core is itself a FreeRTOS
wrapper, but using it hides task/queue control the project explicitly wants. Since a
core project goal is a clean, extensible "firm foundation" suitable for an eventual
open-source release, firmware is built directly on **ESP-IDF with explicit FreeRTOS
tasks** from the very first prototype — not prototyped in Arduino and ported later.
This is more upfront setup but avoids a rewrite and keeps task boundaries honest from
day one.

## FreeRTOS task architecture (starting point — refine during Phase 1)

Suggested separation of concerns as distinct tasks communicating via queues/event
groups (not shared global state):

- **Display task** — owns the SPI bus and all rendering, including the sprite-frame
  animation loop for the currently-selected menu icon. Nothing else touches the display
  directly; other tasks post "render this screen/state" events to it.
- **Input task** — polls the PCF8574T over I2C (buttons, encoder push, 3-way switch)
  and services the native GPIO interrupts for the rotary encoder A/B channels.
  Debounces and translates raw input into semantic events (e.g., `ENCODER_CW`,
  `BUTTON_HYPE_PRESS`, `SWITCH_LOCK`) posted to a shared input event queue.
  Rationale for split GPIO handling: see hardware doc, pin budget section.
- **BLE task** — owns the GATT server/client connection lifecycle, message
  serialization/deserialization, and reconnection logic. Talks to the app; never
  touches Spotify directly (see `app/01-ble-communication.md`).
- **App logic / state machine task** — owns menu navigation state, current screen,
  Hype/Rest timer state, and workout session state. Consumes input events, consumes BLE
  messages, decides what changes, and posts render events to the display task and
  outbound messages to the BLE task. This is the "brain" task.
- **Storage task** (or a protected module, not necessarily a full task) — handles
  reading/writing the on-device flash storage for settings and the 2–3 cached workout
  templates/logs (see Data & storage section below).
- **Power management** — screen dim/sleep timers, lock-state handling, deep-sleep
  logic for the Off transition if hardware allows a soft path, and Phase 0b-driven
  power tuning once real current draw is known.

This is a starting decomposition, not a rigid final spec — expect refinement once
Phase 1 implementation begins.

## Menu navigation model (locked)

- **Rotary encoder rotation** = primary scroll through any list/menu.
- **Rotary encoder push** = primary select/confirm action.
- **3 of the 5 push buttons are fixed, global music controls** (e.g., play/pause,
  next, previous — exact 3 to be finalized when wiring the music control doc, but they
  behave identically regardless of what screen is showing, since music control should
  always be reachable without navigating away from whatever else the user is looking
  at).
- **The remaining 2 buttons (Hype/Rest) are context-dependent on current screen**:
  - On the **Now Playing** and **active workout / guided logging** screens: pressing
    Hype or Rest triggers the actual Hype/Rest snapshot+timer+playlist behavior (see
    `app/02-music-control.md`).
  - On **any menu, settings, or list screen**: the same two physical buttons are
    repurposed as **Back** and **Home/Save** (exact mapping TBD during UI implementation,
    but the principle is locked: buttons are relabeled by screen, not by press-duration).
  - This is a **deliberate design decision over the long-press alternative**: because
    Hype/Rest needs to be instant and impossible to misfire mid-set, disambiguation is
    done by *which screen is currently active* rather than by press timing. No
    long-press detection logic is needed for this — it's a straightforward
    "current screen determines button meaning" lookup in the state machine.

## Top-level home screen (locked)

The home/idle screen is **user-configurable and state-aware**, one of:
1. Combined workout status + music (both visible at once — see the "Current exercise"
   Lopaka screen as the closest existing reference)
2. Music only
3. Workout only

Selection logic: if the device has no active BLE/phone connection, music-only or
combined-with-music options are not meaningful, so the home screen should sensibly fall
back toward workout-only content. If connected, respect the user's configured
preference. This preference is set in the app (and ideally mirrored in on-device
settings for standalone adjustment). The user can still scroll away from the home
screen via the encoder to reach the full menu (Workout Logs / Music Queue / Settings) at
any time.

## Lock / Off state handling (locked — see hardware doc for switch wiring)

- **Off** = hardware power cut, no firmware state to manage.
- **On** = normal operation, all tasks active.
- **Lock** = software state. Display task blanks/sleeps the screen. Input task
  continues reading the switch position (to detect Lock → On) but the app logic task
  discards/ignores all other input events while locked. **Any in-progress timer (Hype,
  Rest, rest-between-sets) is explicitly exempted from this freeze** — the app logic
  task must keep ticking timers, keep the BLE connection alive if present, and keep
  workout state progressing normally regardless of lock state. Only interactive input
  is suppressed.

## Offline-first behavior (locked)

The device must be fully usable as a standalone workout tracker with **zero phone
connection**. This is not a fallback/degraded mode — it's a first-class supported use
case. Practical implications for firmware:
- Workout state (current session, sets logged, timers) lives in RAM/flash on-device,
  never solely in the phone app's memory.
- A small local cache (2–3 most recent workout templates + their last-session data, plus
  all user settings) is stored in on-device flash so a session can be started, guided,
  and logged with no BLE connection at all.
- When a BLE connection becomes available (at session start, mid-session, or after),
  the device syncs newly logged data up to the app and can pull down updated
  templates/settings. Sync should be designed as "reconcile whenever connected," not a
  blocking requirement for any on-device action.
- Music-related features (obviously) require a live connection and should
  gracefully show as unavailable/greyed rather than causing errors when disconnected.

## Data & storage (on-device)

No SD card. On-device flash (ESP32S3 built-in flash, using NVS or a small filesystem
partition such as LittleFS/SPIFFS) stores:
- User settings (Hype/Rest durations, second-press behavior, home screen preference,
  theme/appearance settings if applicable)
- 2–3 most recent workout templates (exercises, target sets/reps/weight, target
  playlist associations if any)
- Last-session and personal-best data needed to render the on-device stats views
  (last session weight/reps, PR) for those cached templates
- Pending sync queue: any workout logs recorded while disconnected, held until the app
  reconnects and confirms receipt

Full workout history, the full template library, and all long-term analytics live in
the app's database — the device only ever holds a rolling working set, by design (keeps
flash usage small and avoids needing an SD card).

## UI rendering — animated selection icons

Menu items each have an associated icon (e.g., a dumbbell icon for a workout menu
item). The **currently-selected** item's icon plays a small sprite-frame animation loop
(e.g., a spinning dumbbell); all other visible items render their icon as a single
static frame. This must be accounted for in the display task's render loop structure
from Phase 1 onward (a timer-driven partial redraw of the active icon's region, rather
than a full-screen redraw on every animation frame, to keep it performant on this
display/MCU combination). The user has existing icon assets/code sourced externally to
reuse here.

## OTA (future phase, noted now for architecture)

Not in the initial phases, but flagged as a planned future capability: OTA push from
the app to the device for things like theme/settings updates. Firmware partitioning
and update mechanism should be picked with awareness that OTA may be added later
(standard ESP-IDF OTA partition scheme is the natural fit — no special action needed
now beyond not actively working against it).

## Extensibility notes (for open-source goal)

- Keep the **display task's rendering calls behind a small internal interface**
  (e.g., `draw_screen(screen_id, state)`) rather than scattering raw SPI/graphics calls
  through business logic — makes swapping the display panel later realistic without
  touching app logic.
- Keep **input events semantic** (`ENCODER_CW`, `BUTTON_HYPE_PRESS`) rather than raw
  pin-level, so swapping the encoder or button wiring later doesn't ripple through the
  state machine.
- Don't abstract further than this for v1 — e.g., no need for a plugin system or
  runtime-configurable pin mapping yet. Abstraction should track actual anticipated
  swap points (display, input hardware), not hypothetical ones.
