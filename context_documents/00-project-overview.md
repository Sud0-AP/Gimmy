# Project Overview — Gym Companion Device

## What this is

A small, pocketable, tactile hardware device used inside the gym as a **distraction-free
remote control for music (via Spotify) and a standalone workout tracker/logger**. It
pairs over BLE with a companion mobile app (Android + iOS) that handles Spotify OAuth
and API calls on the device's behalf, and stores the user's full workout history/templates.

The core design philosophy: **the phone stays in the pocket/bag**. Everything needed
during a focused workout — music control, hype/rest timers, guided workout logging,
progress stats — lives on the device itself. The device is a **macro device**: fully
customizable and configured via the companion app (playlists, workout templates,
button behavior, timer durations), but usable entirely standalone once configured.

This is being built as a personal project first, with an explicit long-term goal of
becoming an **open-source hardware + firmware + app project** others can fork, extend,
or rebuild with different components. Every architectural decision favors a clean,
modular "firm foundation" over the fastest hack — without over-engineering to the point
development stalls.

---

## Core hardware

| Component | Part | Notes |
|---|---|---|
| MCU | Seeed Studio XIAO ESP32S3 | BLE + main compute, runs ESP-IDF/FreeRTOS |
| Display | Waveshare 1.83" IPS LCD, 240×280, SPI, 65K colors | Landscape orientation |
| Scroll input | M274 360° rotary encoder (with push button) | Primary menu navigation |
| Buttons | 5x push buttons | 3 fixed music controls + 2 dual-purpose (Hype/Rest) |
| Power switch | 3-way switch | Off / On / Lock |
| IO expansion | PCF8574T I2C 8-bit GPIO expander | Handles 5 buttons + encoder button + 3-way switch (encoder A/B stay on native GPIO — see hardware doc) |
| Battery | Single protected 3.7V LiPo, 1200–2000mAh, JST-PH connector | Target 4–6+ hrs runtime |
| Charging | TP4056 USB-C module | Single USB-C port exposed on device for charging |
| Enclosure | Custom 3D-printed case (designed after hardware freeze) | Designed by user post hardware-freeze |

- `context_documents/00-project-overview.md` — High-level architecture & phase plan
- `context_documents/01-device-hardware.md` — BOM, pin mappings, power architecture
- `context_documents/02-firmware.md` — ESP-IDF FreeRTOS task breakdown & logic
- `context_documents/04-ui-ux.md` — Design system, screen inventory & conventions
- `context_documents/05-progress.md` — Current milestone checklist & status
- `context_documents/06-display-ui-library.md` — Waveshare LCD driver & Lopaka shim library
- `context_documents/07-ui-interaction-spec.md` — Visual patterns, animations, state machine & per-screen behavior spec
- `context_documents/08-lopaka-screens-explained.md` — How to read Lopaka exports & convert them to firmware

---

## Core firmware

- **ESP-IDF native (not Arduino framework)**, explicit FreeRTOS tasks from day one —
  separate tasks for display rendering, input polling, BLE comms, and workout/timer
  logic, communicating via queues/events. This is a deliberate "good bones" decision
  to support the long-term open-source/extensibility goal.
- Device is the **source of truth for workout state** — logging, timers, and templates
  work fully offline with no phone connection. BLE/phone is required only for Spotify
  music control and for syncing full workout history back to the app's database.
- Menu system driven by a state-machine navigation engine rendering screens designed
  in Lopaka (screens already prototyped by the user — see `app/04-ui-ux.md` for menu map).
- Selected menu items show a subtle sprite-frame animation (e.g., spinning dumbbell
  icon); unselected items render as static icons. This must be part of the render loop
  design from the start, not bolted on later.

Full detail: `hardware/02-firmware.md`

---

## Core app (Android + iOS)

- Handles Spotify OAuth + all Spotify Web API / Spotify Connect calls. The device never
  talks to Spotify directly — it sends lightweight commands to the app over BLE, and the
  app executes them.
- Maintains the full workout template database, full workout history, and syncs a
  rolling subset (2–3 recent workouts + settings) down to the device for offline use.
- Runs a background BLE connection while a session is active: Android via a foreground
  service (persistent notification while connected), iOS via CoreBluetooth's
  background/state-restoration APIs (no notification required, less deterministic
  reconnection timing).
- System volume control: straightforward on Android (silent), best-effort on iOS
  (Apple requires a visible native volume HUD — no way around this).

Full detail: `app/01-ble-communication.md`, `app/02-music-control.md`,
`app/03-workout-logging.md`, `app/04-ui-ux.md`

---

## Flagship feature: Hype / Rest

Two dedicated buttons. Pressing either:
1. Snapshots the current Spotify playback state (track, position, queue/context, shuffle/repeat state).
2. Starts a user-configured playlist and a user-configured countdown timer.
3. On timer expiry, restores the exact prior playback snapshot.

User-configurable per button (in app, and mirrored in on-device settings where practical):
- Timer duration
- Target playlist
- What a *second press during an active Hype/Rest* does — reset the timer, reshuffle
  the playlist, or cancel the mode entirely (user picks the behavior)

Hype/Rest buttons are **context-dependent**: on the Now Playing / workout screens they
trigger Hype/Rest; inside any menu/settings screen they act as Back/Save instead. This
screen-based disambiguation (not press-duration-based) was chosen deliberately to avoid
accidental triggers mid-set — see `hardware/02-firmware.md` for the state rule.

---

## Guiding principles for all future work on this project

1. **Device state survives disconnection.** Never design a feature that breaks if BLE drops mid-workout.
2. **No text entry on-device.** All template/library creation happens in the app; the device only selects, scrolls, and logs numeric values via the encoder.
3. **Extensible, not over-abstracted.** Code should be modular enough to swap a component (screen, input method, music service) without a full rewrite — but don't add abstraction layers that aren't earning their complexity yet.
4. **Screen-based context, not gesture complexity.** Prefer "what screen am I on" logic over long-press/multi-press disambiguation wherever possible, given the no-look, mid-set usage context.
5. **iOS platform limits are real constraints, not bugs.** Volume control and background BLE behave differently on iOS by Apple's design — document and design around this rather than fighting it.

---

## Phase plan (high level — see individual docs for phase-specific detail)

- **Phase 0a — Core hardware bring-up**: MCU + screen + IO expander + encoder on breadboard, verified.
- **Phase 0b — Power system validation**: Battery + TP4056 wiring, real current draw measurement, runtime sanity check.
  - → **Hardware freeze milestone.** Case design can begin in parallel from this point.
- **Phase 1 — Firmware skeleton**: ESP-IDF project structure, FreeRTOS tasks, menu navigation engine rendering the existing Lopaka screens with dummy data.
- **Phase 2 — BLE protocol + app skeleton**: GATT service/schema defined and implemented, minimal app shell with Spotify OAuth wired up, dummy command round-trip working.
- **Phase 3 — Music control (real)**: Live Spotify playback control, playlist sync, queue display, full Hype/Rest snapshot+restore.
- **Phase 4 — Workout logging**: Template creation in app, sync to device, guided workout flow with logging, on-device stats, offline local storage.
- **Phase 5 — Power, polish, lock/off states, on-device settings, OTA groundwork.**
- **Phase 6 — Enclosure integration + open-source packaging (README, build docs, cleanup).**
