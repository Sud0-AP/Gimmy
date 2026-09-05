# Progress Tracker — Gym Companion Device

## How to use this doc

- Phase structure below mirrors the phase plan in `00-project-overview.md`.
- When starting work on a phase, add concrete tasks under it as checkboxes (`- [ ]`)
  as they come up — this doc is meant to grow organically, not be fully planned
  upfront.
- Check items off (`- [x]`) as completed. Leave a short note inline if something
  needed a decision or deviated from the original doc (and update the relevant
  project doc itself if it's a locked decision, not just a task).
- **Current phase** at the top should be kept up to date — that's the fastest way
  to see where things stand at a glance.
- The **Key decisions & learnings log** at the bottom is a running changelog of
  anything that corrected or added to the other docs, with a one-line summary and
  which doc it landed in — full detail lives in that doc, not duplicated here.

---

## Current status

**Phase: 0a — Core hardware bring-up** (in progress)

---

## Phase checklist

### Phase 0a — Core hardware bring-up
- [x] LCD wired to XIAO ESP32S3 over SPI, rendering confirmed (ported vendor NV3030B demo)
- [x] Rotary encoder wired to native GPIO, direction decode confirmed (single CHANGE-interrupt method) — now on D6/D7 after freeing D4/D5 for the expander
- [x] IO expander (PCF8574T) wired, all inputs confirmed reading correctly over I2C (address 0x20)
- [x] Push buttons (5x) wired via expander, confirmed working — mapping: Previous=P2, Play/Pause=P4, Next=P0, Hype=P3, Rest=P1
- [x] Encoder push-button wired and confirmed
- [ ] 3-way switch state detection confirmed for all 3 positions — deferred, wiring not yet done

### Phase 0b — Power system validation
- [ ] Battery + TP4056 wiring complete — blocked on battery arriving
- [ ] Real current draw measured (screen on + BLE idle, screen on + BLE active)
- [ ] Runtime estimate recalculated from measured current draw, confirmed ≥3–4hr minimum
- [ ] **Hardware freeze milestone** — case design can start once this phase closes

### Phase 1 — Firmware skeleton
- [ ] ESP-IDF project structure set up (migrate off Arduino bring-up sketches)
- [ ] FreeRTOS tasks scaffolded: display, input, BLE, app-logic, storage
- [ ] `draw_screen(screen_id, state)` interface defined
- [ ] Lopaka screens ported with dummy data:
  - [ ] Main menu
  - [ ] Now Playing / current exercise (combined home screen)
  - [ ] Choose Split
  - [ ] Choose Workout
  - [ ] Workout overview ("Push A")
  - [ ] Guided logging
  - [ ] Exercise stats
  - [ ] Music Queue
- [ ] Animated selection icon (sprite-frame loop) working in render loop

### Phase 2 — BLE protocol + app skeleton
- [ ] GATT service/schema defined (message format, version field)
- [ ] Minimal app shell created, Spotify OAuth wired up
- [ ] Dummy command round-trip working (device ⇄ app)

### Phase 3 — Music control (real)
- [ ] Live Spotify playback control
- [ ] Playlist shortlist sync + picker
- [ ] Queue display (Music Queue screen live data)
- [ ] Hype/Rest snapshot + timer + restore, full flow
- [ ] Volume control (Android silent, iOS HUD)

### Phase 4 — Workout logging
- [ ] Template creation in app
- [ ] Template sync to device (2–3 recent cache)
- [ ] Guided workout flow with logging (encoder weight/reps interaction — needs UX decision, see `03-workout-logging.md` open questions)
- [ ] On-device stats screen (last session + PR)
- [ ] Offline local storage + sync-on-reconnect

### Phase 5 — Power, polish, lock/off states, settings, OTA groundwork
- [ ] Screen dim/sleep power-saving behavior
- [ ] Lock state (timers keep running, input suppressed)
- [ ] On-device Settings screen (see `04-ui-ux.md` for field list)
- [ ] OTA partition scheme in place (no transport work yet)

### Phase 6 — Enclosure integration + open-source packaging
- [ ] Case design (post hardware-freeze)
- [ ] README, build docs, cleanup for open-source release

---

## Component sourcing log

| Part | Status | Notes |
|---|---|---|
| XIAO ESP32S3, display, rotary encoder, push buttons | In hand | |
| PCF8574T IO expander | In hand, wired and confirmed | Address 0x20 (all address pins low) |
| 3-way switch, 2P2T (DPDT) ON-OFF-ON | In hand, not yet wired | |
| Battery: NOVA 604060, 2000mAh | In hand, not yet wired | |
| Antenna | No purchase needed | Stock XIAO U.FL stick antenna is sufficient for short-range BLE |

---

## Key decisions & learnings (running log)

- **Battery**: corrected an initial wrong assumption about owned WLY551145 cells
  being unprotected (they're PCM protected); ultimately went with a fresh NOVA
  604060 2000mAh cell instead, since only 2 owned cells (~500mAh combined) fell
  short of the runtime target. See `01-device-hardware.md`.
- **Power switch**: confirmed 2P2T (DPDT), not 1P2T — needed for true hardware
  power cutoff plus separate On/Lock sensing. See `01-device-hardware.md`.
- **Antenna**: stock XIAO U.FL antenna is sufficient; no separate dipole antenna
  needed for short-range BLE.
- **LCD wiring**: CS must not share XIAO's default hardware MISO pin (D9) —
  moved to D0. Full confirmed pin table now in `01-device-hardware.md`.
- **Encoder decode**: a single CHANGE interrupt on CLK (compared against DT's
  state) is enough to decode direction — no need for interrupts on both channels.
  Noted in `02-firmware.md`.
- **Encoder relocation**: moved off D4/D5 to D6/D7 once the PCF8574T expander
  arrived and needed the I2C pins. See `01-device-hardware.md`.
- **PCF8574T confirmed**: I2C address 0x20, buttons wired common-GND with one
  dedicated expander pin each (no external pull-ups needed). Button mapping
  confirmed and logged in `01-device-hardware.md`.
- **`Paint_NewImage` argument order**: the Waveshare GUI_Paint library expects the
  *physical* panel dimensions (240, 280) as its Width/Height args regardless of
  rotation — not the logical rotated canvas size. Passing the rotated dimensions
  directly corrupts `Paint_Clear`'s addressing and scrambles rendering. Correct
  call for landscape use: `Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_90, ...)`.
