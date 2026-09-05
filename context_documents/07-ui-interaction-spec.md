# UI Interaction Specification — Patterns & Per-Screen Behavior

This document provides the complete UI interaction specification for the Gym Assistant device, combining:
1. **Visual patterns** — the selection mechanisms, animation rules, and input mapping used across all screens
2. **Per-screen behavior** — exact encoder/button behavior, entry/exit points, and component state for every screen

This is a **gated** spec for Phase 1 implementation: **no firmware code until every in-scope screen is marked `[x]` Done below AND the human explicitly says to proceed.**

**Process rule**: when discussing a screen, if anything is ambiguous, contradicts another doc, or has more than one reasonable interpretation — ask and get it resolved first. Never write speculative or half-resolved behavior into this doc. An explicit open question is better than a guessed answer.

**To continue this discussion in a new conversation**, upload:
1. This doc (`context_documents/07-ui-interaction-spec.md`)
2. `context_documents/08-lopaka-screens-explained.md`
3. The relevant screen's `.txt` code + `.png` image from `Hardware/Display/Lopaka Screens/`
4. Any project doc relevant to that screen (`02-music-control.md`, `03-workout-logging.md`, `04-ui-ux.md`, etc.)

---

## Part 1: Visual Patterns & Global Input Mapping

### 1.1 Visual Selection & Highlighting Patterns

Lopaka screens express interactive focus using four primary mechanisms:

#### Pattern A: Bounding Box / Outline Color Swap (Lists & Menus)
- **Unselected item**: Thin border `0x506C` (dark gray), secondary text color `0x506C` or `0x6870`, static icon.
- **Selected item**: Thick / bright rounded outline `0xE33F` (bright magenta) or `0xFFFF` (white), text becomes `0xE33F` or `0xFFFF`, and the icon slot runs an active **sprite-frame animation**.
- **Examples**:
  - Main Menu (`menu_1.txt`, `menu_2.txt`, `menu_3.txt`):
    - `menu_1`: "Workout Logs" text is `0xE33F`, selection pill `image_Layer_28_bits` rendered at `y=25` with color `0xE33F`.
    - `menu_2`: "Music Queue" text is `0xE33F`, selection pill rendered at `y=97`.
    - `menu_3`: "Settings" text is `0xE33F`, selection pill rendered at `y=169`.
  - Choose Split (`log_workout_1.txt`):
    - Selected split ("Push Pull Legs") gets highlighted rounded background pill (`image_Layer_21_bits` @ `y=116`, color `0xE33F`) and text `0xE33F` with play arrow icon `image_download_1_bits`. Unselected splits use `0x506C`/`0x6870` and gray outline boxes.

#### Pattern B: Selection Dot / Ellipse Fill (Back, Info & Header Icons)
- **Unselected state**: `back_selection_color = 0x801` (matches screen background color, meaning invisible / transparent).
- **Selected state**: `back_selection_color = 0xE33F` or `0xFFFF` (draws an ellipse/circle indicator around or under the icon).
- **Example**:
  ```cpp
  int back_selection_color = 0x801;  // Near-black = unselected
  int Info_selection_color = 0x801;  // Near-black = unselected
  tft.drawEllipse(13, 10, 8, 6, back_selection_color);  // Draws highlight when selected
  tft.drawEllipse(40, 10, 8, 6, Info_selection_color);
  ```

#### Pattern C: Coordinate Offset / Scroll Indicator (Scrollbars)
- Scroll position is represented by vertical slider blocks:
  - Menu scrollbar track: `tft.fillRect(264, 32, 12, 194, 0x6870);`
  - Active thumb slider: `tft.fillRect(265, thumb_y, 9, 15, 0xE33F);`
  - `thumb_y` moves depending on the active index:
    - Item 0 (Workout Logs): `y = 59`
    - Item 1 (Music Queue): `y = 106`
    - Item 2 (Settings): `y = 153`

#### Pattern D: Set Marker in Guided Logging
- In active workout view (`Home_screen_both_1.txt`):
  - `int current_set_marker_y = 140;`
  - `tft.drawRect(148, current_set_marker_y, 124, 13, 0x6870);`
  - Moving to Set 1, 2, 3, 4 shifts `current_set_marker_y` to `113, 126, 140, 154, 168` etc.

### 1.2 Animation Placeholders & Sprite Placement

In Lopaka screen layouts, square bounding boxes (e.g. `48×48`, `32×32`, `64×64`) mark where animations go:

| Location | Dimensions | Purpose | Associated Animation File |
|---|---|---|---|
| Main Menu Item 0 Icon Slot | `48×48` @ `(8, 36)` | Selected "Workout Logs" icon | `Animations/dumbbell_animation.txt` |
| Main Menu Item 1 Icon Slot | `48×48` @ `(8, 108)` | Selected "Music Queue" icon | `Animations/settings_animation.txt` (or music note) |
| Main Menu Item 2 Icon Slot | `48×48` @ `(8, 181)` | Selected "Settings" icon | `Animations/settings_animation.txt` |
| Split/Workout Select Icon | `64×64` @ `(13, 105)` | Muscle / Split Graphic | `Animations/bench_press_animation.txt` / `deadlift_animation.txt` |
| Home Screen Active Exercise | `32×32` @ `(151, 35)` | Mini exercise animation | `Animations/dumbbell_animation.txt` |

#### Rendering Rule:
- **When item is UNSELECTED**: Draw only frame 0 (static bitmap).
- **When item is SELECTED**: The display task loops through all frames of the sprite array at ~24 FPS (`delay = 42ms`), doing a **partial redraw** of that `W×H` bounding box without refreshing the entire screen.

### 1.3 Global Input Mapping

#### Physical Inputs:
1. **Rotary Encoder Rotation (`CW` / `CCW`)**: Moves selection cursor in list/menu (increments/decrements focused element index).
2. **Rotary Encoder Push (`SELECT`)**: Activates currently focused element.
3. **Music Buttons (3 fixed)**:
   - Button 1: Previous Track / Rewind
   - Button 2: Play / Pause Toggle
   - Button 3: Next Track / Skip
4. **Context Buttons (2 buttons - Hype & Rest)**:
   - **On Home / Workout screens**: Triggers Hype Timer or Rest Timer.
   - **On Menus / Settings / List screens**: **Hype (left) = Back**, **Rest (right) = Info**. Not encoder-reachable.

### 1.4 Screen Tree & State Hierarchy

```
[HOME SCREEN] (Combined / Music Only / Workout Only)
  │
  ├── Encoder CW/CCW ────────► [MAIN MENU]
  │                              ├── 1. Workout Logs ──► [CHOOSE SPLIT]
  │                              │                         └── [CHOOSE WORKOUT]
  │                              │                               └── [WORKOUT OVERVIEW]
  │                              │                                     └── [ACTIVE LOGGING SESSION]
  │                              │                                           ├── [EXERCISE STATS / PR]
  │                              │                                           └── (Back / Finish)
  │                              ├── 2. Music Queue ───► [MUSIC QUEUE VIEW]
  │                              │                         ├── Shuffle toggle
  │                              │                         ├── Repeat toggle
  │                              │                         └── Track List
  │                              └── 3. Settings ──────► [SETTINGS MAIN]
  │                                                        ├── Display (Brightness/Timeout)
  │                                                        ├── Hype & Rest Durations
  │                                                        ├── Pairing / Bluetooth
  │                                                        └── Device Info
```

### 1.5 Coding Model for Firmware

When building firmware code to render and navigate these screens:

1. **State Structure**:
   ```cpp
   struct MenuState {
       uint8_t selectedIndex;     // 0 = Item 1, 1 = Item 2, etc.
       uint8_t totalItems;        // Number of items in list
       bool isDirty;              // Requires redraw
       uint8_t animFrame;         // Current animation frame index
   };
   ```

2. **Parametric Drawing Functions**:
   Instead of hardcoding every variant as a separate monolithic function, convert Lopaka screens into parameterized components:
   ```cpp
   void drawMainMenu(uint8_t selectedIndex, uint8_t animFrame) {
       tft.fillScreen(0x801);
       drawTopBar("12:23 PM", "14th May Monday", true, 85);
       
       // Draw items
       drawMenuItem(0, "Workout Logs", 52, selectedIndex == 0, animFrame);
       drawMenuItem(1, "Music Queue", 125, selectedIndex == 1, animFrame);
       drawMenuItem(2, "Settings", 198, selectedIndex == 2, animFrame);
       
       // Draw scrollbar thumb based on selectedIndex
       int thumbY = 59 + (selectedIndex * 47);
       tft.fillRect(265, thumbY, 9, 15, 0xE33F);
   }
   ```

3. **Partial Updates for Smooth Animation**:
   Only clear and redraw the 48×48 sprite bounding box when ticking animation frames to eliminate display flicker and preserve SPI bandwidth.

---

## Part 2: Per-Screen Behavior Specification

### Status legend
- `[ ]` Not started
- `[~]` In progress / partially discussed, open questions remain
- `[x]` Done — behavior fully specified below

### Screen checklist

#### Home
- [x] Home screen — combined (workout + music), both_1/both_2 variants
- [x] Home screen — workout only
- [x] Home screen — music only

#### Main Menu
- [x] Main Menu (single screen, 3 selection states)

#### Workout Logs branch
- [x] Choose Split (`log_workout_1`)
- [x] Choose Workout (`log_workout_2`)
- [x] Workout Overview / "Push A" (`log_workout_3`)
- [x] Guided Logging / active session (`log_workout_4`)
- [x] Exercise Stats (`log_workout_5`)

#### Music branch
- [x] Music Queue

#### Settings branch
- [x] Settings main
- [x] Settings — Display
- [x] Settings — Hype & Rest
- [x] Settings — Brightness
- [ ] Settings — Pairing (not yet mocked)
- [ ] Settings — Theme (not yet mocked)
- [ ] Settings — Home screen preference (not yet mocked)

#### Cross-cutting / not tied to one screen
- [~] Top bar component (time/date, BT status, battery) — disconnected/low battery behavior not yet discussed
- [ ] Global music button behavior (the 3 fixed buttons, reachable from every screen)
- [x] **Hype/Rest button behavior on menu/list/settings screens (resolved)**: **Hype (left) = Back**, **Rest (right) = Info**. Not encoder-reachable/scrollable. Back navigates up one level; Info opens a transient overlay with contextual help (dismissed by pressing Info again or Back).
- [ ] Navigation stack rules (what "Back" returns to from each depth)
- [x] **Global text overflow rule**: every text element has a fixed-width bounding box. If the string doesn't fit, truncate with "..." and marquee-scroll to reveal the rest every few seconds.
- [x] **Global carousel scroll-bounce indicator**: on carousel screens (Choose Split, Choose Workout), encoder rotate triggers a brief directional pill nudge + spring-back for tactile feedback.

### Current focus

Workout Logs branch, Music branch, and four Settings screens (Main, Display, Hype & Rest, Brightness) are Done. Remaining: Settings Pairing/Theme/Home-screen-preference (unmocked), top bar disconnected/low-battery states, global music button behavior, nav-stack rules.

---

## Screens discussed so far

### Home screen — combined (both_1 / both_2 variants)

**Status:** Done

**Purpose:** default/idle screen when a workout is in progress or music is playing
(or both). Shows workout progress and music playback side by side.

**Entry points:** device idle/boot default; landed on via Home/Back navigation from
elsewhere (exact trigger TBD once we cover navigation stack rules).

**Components on screen:**
- Top bar: time, date, optional "Monday: Push" workout-day label (only in `both_1`;
  `both_2` omits it), Bluetooth status icon, battery icon
- Large left box: **album cover of the currently playing song** (see open question 1
  below — this conflicts with the earlier "no arbitrary album art" note in
  `02-music-control.md`)
- Small box next to "Current exercise:" label: **animated stickman icon**, novelty
  animation that changes based on the current exercise's target muscle group (e.g.
  bench-press animation for chest exercises, deadlift animation for back exercises).
  This is a *different* animation category than the menu-selection sprite loop —
  it's tied to muscle-group content, not to selection state.
- Exercise info box: current exercise name, target muscle, total sets×reps summary
  (e.g. "chest" / "4×10-12")
- Set list (warmup + work sets): sets the user has already logged show the actual
  logged value as static text (e.g. "Set 1: 20 Kg X 9"); the current/next unlogged
  set is highlighted with a box and shows the *target* rep range (e.g. "Set 2: 20 Kg
  X 8-10"); sets further out show target range as plain text
- Music block: track progress bar (elapsed/total time), track title, artist name
- Hype/Rest indicator: shows the current mode name (HYPE or REST) with icon and a
  live countdown timer while a mode is active; when no mode is active, shows the
  **last used mode**, with a static (non-ticking) time value

**Encoder rotate (CW/CCW):** Home screen itself has no selectable list. Any
rotation (either direction) transitions to the Main Menu, always opening with
**"Workout Logs" pre-selected** (fixed default — direction does not matter for this
specific transition, and the Menu does not remember its last-selected item).

**Encoder press:** context-sensitive shortcut, bypassing the Main Menu:
- If a workout is currently active → jumps straight to Guided Logging screen
- If no workout is active → jumps straight to Music Queue screen

**Mode indicator (resolved):** the HYPE/REST box is a single generic "current mode"
indicator, not two separate fixed controls. It dynamically shows whichever mode
(HYPE or REST) is currently active or was last active, with the matching icon,
label, and timer — this applies the same way on all three Home variants. The
combined mockup happening to show HYPE, workout-only happening to show HYPE, and
music-only happening to show REST were just representative/dummy states, not a
per-variant display rule.

**Other buttons active on this screen:** Hype/Rest always trigger their real
Hype/Rest behavior here (never repurposed as Back — see Exit points). The 3 fixed
music buttons behave exactly as everywhere else: send the command, screen reflects
the change once the app/BLE round-trip completes — nothing Home-specific.

**Exit points (resolved):** Home is purely a root screen — there is no Back action
from Home at all. The only ways to leave are forward: encoder rotate → Main Menu, or
encoder press → context shortcut (Guided Logging or Music Queue).

**Resolved — album art (real, not placeholder):** the big box renders the actual
currently-playing track's album art, fetched via Spotify. This is a confirmed scope
change from the earlier "fixed local icons only" decision in `02-music-control.md`
and the "no arbitrary image transfer" lean in `01-ble-communication.md` — **both of
those docs need updating once the BLE transfer approach is finalized.** The exact
transfer format (app sends a ready-to-draw raw bitmap vs. app sends compressed JPEG
that the device decodes) is explicitly **tabled for the BLE-protocol discussion**
(Phase 2 scope) — for now, "Home screen displays real synced album art" is locked
as a requirement, the mechanism is not yet decided. See "Flagged for other docs"
section at the bottom of this doc.

---

### Home screen — workout only

**Status:** Done — inherits all resolved behavior from the combined variant
(mode indicator, encoder rotate/press, exit points, fixed music buttons all
identical; only visual layout differs)

**Purpose:** Home screen fallback when no music/phone connection is relevant —
larger dedicated space for exercise info.

**Components on screen (from screenshot):**
- Top bar: time, date (no workout-day label shown here), BT, battery
- Empty box top-left (likely same animation slot as combined variant, unconfirmed)
- Large exercise name as the screen's headline (e.g. "Incline dumbbell Press")
- "Current set:" label + same set list pattern as combined variant
- HYPE button + timer box (no REST box shown — see open question 3 above)
- "Target Muscles:" line at the bottom (e.g. "chest, front dealt, triceps") — new
  element not present in the combined variant's Home screen

**Resolved:** the HYPE box here follows the same generic "current mode indicator"
rule as the combined variant (see above) — it's not workout-only-specific, it would
show REST too if that were the active/last mode.

**Confirmed:** encoder rotate/press, exit points, and fixed music button behavior
are all identical to the combined variant — no screen-specific differences.

---

### Home screen — music only

**Status:** Done — inherits all resolved behavior from the combined variant
(mode indicator, encoder rotate/press, exit points, fixed music buttons all
identical; only visual layout differs)

**Purpose:** Home screen fallback when workout context isn't relevant (no active
workout) — dedicated space for music + upcoming queue.

**Components on screen (from screenshot):**
- Top bar: time, date, BT, battery (no workout-day label)
- Large box top-left (same album-art / placeholder slot as combined variant — same
  open question #1 applies)
- Track title + artist (icons reused from combined variant)
- "Up next:" preview list showing the next **2** upcoming tracks with artist —
  relationship to the standalone Music Queue screen (which shows 5–10 upcoming
  tracks per `02-music-control.md`) not yet clarified — same data, different
  preview length? Or does music-only Home always show exactly 2 regardless of
  Music Queue screen's own scroll state?
- Track progress bar (elapsed/total)
- REST button + timer box (no HYPE box shown — see open question 3 above)

**Resolved:** the REST box here follows the same generic "current mode indicator"
rule as the combined variant (see above) — not music-only-specific, it would show
HYPE too if that were the active/last mode.

**Confirmed:** "Up next" shows the same underlying queue as the standalone Music
Queue screen — Home just previews the first 2 entries of it, Music Queue shows
5–10 with full scroll. Encoder rotate/press, exit points, and fixed music button
behavior are all identical to the combined variant.

---

### Main Menu

**Status:** Done

**Purpose:** central hub screen — routes to the 3 top-level branches (Workout Logs,
Music Queue, Settings).

**Entry points:**
- From Home, via encoder rotate (either direction) → always opens with **Workout
  Logs pre-selected**, regardless of what was selected last time this screen was
  visited.
- From any submenu's Back action tracing back up to Main Menu → **retraces its
  steps**, re-selecting whatever item was selected when the user left (not reset to
  Workout Logs). Only the Home → Menu entry path forces the reset to Workout Logs;
  every other entry path preserves last-selected state.

**Components on screen:**
- Top bar: time, date, BT status, battery (same shared component as Home)
- Header ↩ (back) and ? (info) icons — static/decorative on this screen, **not**
  encoder-reachable (see Hype/Rest note below — these are driven by the physical
  Hype/Rest buttons instead)
- 3 vertically stacked list items: Workout Logs / Music Queue / Settings, each with
  an icon box
- Selected item: thick magenta pill outline, bright text, animated icon (sprite
  loop, matches the general selection-animation rule from `04-ui-ux.md`/
  `02-firmware.md`)
- Unselected items: thin gray outline, dim text, static icon frame 0
- Scrollbar thumb on the right, position tracks `selectedIndex` (Pattern C, per
  `08-lopaka-screens-explained.md`)

**Encoder rotate:** scrolls `selectedIndex` up/down through the 3 items. **Stops
(clamps) at the boundaries** — no wraparound past Workout Logs or past Settings.

**Encoder press:** navigates to the selected branch:
- Workout Logs → Choose Split (`log_workout_1`)
- Music Queue → standalone Music Queue screen
- Settings → Settings main

**Other buttons active on this screen:** Hype/Rest are repurposed here per the
general cross-cutting rule (see Cross-cutting section above) — **Hype (left) =
Back → Home**, **Rest (right) = Info → contextual help overlay**. The 3 fixed music
buttons behave as everywhere else (global, always reachable, no Main-Menu-specific
behavior).

**Open questions:** none remaining for this screen.

---

### Choose Split (`log_workout_1`)

**Status:** Done

**Purpose:** Select active split template (e.g., Push Pull Legs, Upper Lower, Bro Split, Custom splits).

**Entry points:**
- From Main Menu ("Workout Logs" selected) when no workout routine has been established yet.
- From Screen 3 ("Change Workout" action pressed).

**Components on screen:**
- Top bar: time, date, BT status, battery (shared standard component).
- Header ↩ (back) and ? (info) icons (driven by Hype/Rest physical buttons).
- Screen title: "Choose Split:" (size 3) at `(15, 50)`.
- 64×64 animated sprite slot at `(13, 105)`: animated stickman/muscle sprite loop (same sprite category for both Screen 1 and Screen 2).
- 5 visible item slots: exactly 5 slots rendered on screen at fixed coordinates (`y=95`, `116`, `146`, `167`, `191`).
- Selection highlight: **Slot 2 (middle position, `y=116`) is fixed** as the selected slot with magenta pill outline (`image_Layer_21_bits`), bright magenta text, and left arrow icon (`image_download_1_bits`). Unselected slots show dim gray outlines (`image_download_bits`) and dim text.

**Encoder rotate (CW/CCW):** Carousel scroll — the highlight pill stays fixed in slot 2 while the underlying split list text shifts behind it. **Wraps around** past the first and last item. Implicit scrolling (no visible arrows or scrollbars).

**Encoder press:** Selects the highlighted split → navigates to **Choose Workout (`log_workout_2`)** pre-loaded with that split's specific workouts.

**Other buttons active on this screen:** Hype (left) = Back → Main Menu; Rest (right) = Info → contextual help overlay. Global music buttons behave as usual.

**Exit points:** Hype/Back → Main Menu. Encoder press on highlighted split → `log_workout_2`.

**Open questions:** none remaining for this screen.

---

### Choose Workout (`log_workout_2`)

**Status:** Done

**Purpose:** Select a specific workout routine under the chosen split (e.g., Push A, Pull A, Legs A, Push B).

**Entry points:** From `log_workout_1` (Choose Split) upon encoder press on a split.

**Components on screen:**
- Top bar: Standard.
- Header ↩ (back) and ? (info) icons.
- Screen title: "Choose workout:" (size 3) at `(10, 50)`.
- Parent split context label at bottom-left: e.g. "Push Pull Legs:" at `(5, 180)` (shows which split is active).
- 64×64 animated sprite slot at `(15, 105)`: Reuses the same animated stickman/muscle sprite as Screen 1.
- 5 visible item slots: Fixed center carousel highlight in slot 2 (`y=116`), identical layout and visual styling to Screen 1.

**Encoder rotate (CW/CCW):** Carousel scroll through the routine list for the selected split (wraps around).

**Encoder press:** Selects the routine → navigates to **Workout Overview (`log_workout_3`)**.

**Other buttons active on this screen:** Hype (left) = Back → `log_workout_1`; Rest (right) = Info → contextual help overlay. Global music buttons behave as usual.

**Exit points:** Hype/Back → `log_workout_1`. Encoder press → `log_workout_3`.

**Open questions:** none remaining for this screen.

---

### Workout Overview / "Push A" (`log_workout_3`)

**Status:** Done

**Purpose:** Dashboard overview showing the full list of exercises in the selected routine, with options to start the full workout, log an individual exercise directly, or change the workout routine.

**Entry points:**
- From `log_workout_2` after selecting a routine.
- Directly from Main Menu ("Workout Logs") once a routine has been chosen previously (Screen 3 acts as the **persistent workout anchor**).
- Returning from single-exercise logging in `log_workout_4`.

**Components on screen:**
- Top bar: Standard.
- Header ↩ (back) and ? (info) icons.
- Workout headline: Routine name (e.g. "Push A") + muscle group subtitle (e.g. "Chest, Shoulders, Tricep").
- Header action buttons:
  - "Start" button (`start_button.png` / `start_button_selected.png`) at `(118, 27)`.
  - "Change Workout" button (`change_button.png` / `change_button_selection.png`) at `(182, 27)`.
- Exercise scroll list: Each exercise item has exercise name, target muscle subtitle, and far-left indicator arrow (`image_arrow_5_bits`).
- Selection highlight on exercise rows: Magenta pill outline (`selection12.png` / `list12.png`) + bright magenta indicator arrow (`selection_pointer12.png`).
- Scrollbar on right edge: Thumb (`scroll_bar_y`) and scroll lines (`scroll_lines_y`) tracking list position.

**Encoder rotate (CW/CCW):** Cycles sequentially through all interactive items:
`[0] Start` ↔ `[1] Change Workout` ↔ `[2] Exercise 1` ↔ `...` ↔ `[N] Exercise M` ↔ wraps back to `[0] Start`. Scrollbar thumb tracks the active list index.

**Encoder press:**
- On "Start": Starts the full workout session → opens **Guided Logging (`log_workout_4`)** focused on the first pending exercise.
- On "Change Workout": Clears the active selection → navigates back to **Choose Split (`log_workout_1`)**.
- On an individual Exercise row: Opens **Guided Logging (`log_workout_4`)** focused specifically on logging that single exercise. When completed or exited, returns directly back to Screen 3.

**Other buttons active on this screen:** Hype (left) = Back → Main Menu; Rest (right) = Info → contextual help overlay. Global music buttons behave as usual.

**Exit points:**
- Hype/Back → Main Menu.
- "Start" → `log_workout_4` (full workout flow).
- "Change Workout" → `log_workout_1`.
- Individual exercise press → `log_workout_4` (single exercise flow, returning to Screen 3).

**Open questions:** none remaining for this screen.

---

### Guided Logging / Active Session (`log_workout_4`)

**Status:** Done

**Purpose:** Active in-session workout tracking for sets, repetitions, and weights for each exercise.

**Entry points:**
- From Screen 3 ("Start" pressed for sequential workout, or individual exercise pressed).
- From Home screen shortcut (encoder press when a workout is active).

**Components on screen & PNG Assets:**
- Top bar: Standard.
- Header line 1: Routine name ("Push A") at `(10, 32)` + Top navigation buttons:
  - "Stop" button (`stop_button.png` / `stop_button_selected.png`) at `(122, 30)` to end the workout.
  - "Home screen" button (`home_button.png` / `home_button_selected.png`) at `(182, 30)` to background the workout session.
- Header line 2: Exercise name (e.g. "1. Incline Dumbbell Press") at `(10, 57)` + "Stats" button (`info_button_bits` / `info_button_selected.png`) at `(200, 60)`.
- Set table: Columns for `Set number` (`x=25`), `Weight` (`x=122`), and `Repetitions` (`x=193`) with 5 set rows (Warmup 1-2, Work Set 1-3).
- Row selector: `set_pointer.png` (264×18 horizontal box spanning the row).
- In-place edit highlights: `selected_set_weight.png` (weight box), `selected_set_reps.png` (reps box), `selected_set_save.png` (check/save button).
- Bottom navigation buttons:
  - "Previous" button (`previous_button.png` / `previous_button_selected.png`) at `(15, 216)`.
  - "Next" button (`next_button.png` / `next_button_selected.png`) at `(212, 216)`.

**Encoder rotate (Navigation Mode):**
Moves selection pointer up/down across:
`Home screen` ↔ `Stop` ↔ `Stats` ↔ `Set 1` ... `Set 5` ↔ `Previous` ↔ `Next` (wraps around).
- Selected buttons use their respective `*_selected.png` highlighted assets.
- Default focus when entering screen is on the current unlogged set row (`set_pointer.png`).

**Encoder press:**
- **On a Set Row**: Enters **In-Place Edit Mode**:
  1. **Weight Edit**: `selected_set_weight.png` activates around the weight value → Rotate encoder to adjust weight → Press encoder to confirm and advance.
  2. **Reps Edit**: `selected_set_reps.png` activates around the rep count → Rotate encoder to adjust reps → Press encoder to confirm and advance.
  3. **Save/Commit**: `selected_set_save.png` activates → Press encoder to save set → Commits logged data, marks set completed, and auto-advances `set_pointer.png` to the next pending set row.
  *(Hype/Back during edit cancels without saving and returns to row navigation).*
- **On `Stats`**: Opens **Exercise Stats (`log_workout_5`)**.
- **On `Home screen`**: Leaves workout running in background → navigates to Home screen.
- **On `Stop`**: Ends workout session → returns to Workout Overview (`log_workout_3`).
- **On `Previous` / `Next`**: Advances to previous / next exercise in routine.

**Other buttons active on this screen:** Hype (left) = Back → Workout Overview (`log_workout_3`); Rest (right) = Info → contextual help overlay. Global music buttons behave as usual.

**Exit points:**
- Hype/Back or `Stop` → `log_workout_3`.
- `Home screen` → Home screen (workout active in background).
- `Stats` → `log_workout_5`.
- Finishing last set / Next on last exercise → `log_workout_3` (workout complete).

**Open questions:** none remaining for this screen.

---

### Exercise Stats (`log_workout_5`)

**Status:** Done

**Purpose:** Read-only modal displaying Personal Bests, Last Session stats, and execution notes for the current exercise.

**Entry points:** From "Stats" button on Screen 4 (`log_workout_4`).

**Components on screen:**
- Top bar: Standard.
- Header: Exercise name + target muscle group.
- "Notes:" field (e.g. "Notes: 30 degree incline").
- "Last session:" summary table (Date, Sets, Weight, Reps).
- "Personal best:" summary table (Date, Sets, Weight, Reps).
- "Done / Back" action button (`image_Layer_34_pixels`) at `(115, 215)`, selected by default.

**Encoder rotate (CW/CCW):** No scrolling needed (read-only modal view with a single action button).

**Encoder press:** Returns to **Guided Logging (`log_workout_4`)**.

**Other buttons active on this screen:** Hype (left) = Back → `log_workout_4`; Rest (right) = Info → contextual help overlay. Global music buttons behave as usual.

**Exit points:** Hype/Back or Encoder press on "Done" → `log_workout_4`.

**Open questions:** none remaining for this screen.

---

### Music Queue

**Status:** Done

**Purpose:** Read-only "now playing" status plus a scrollable, interactive queue —
lets the user jump to any upcoming track and toggle shuffle/repeat, without needing
the phone.

**Entry points:**
- Main Menu → "Music Queue".
- Home screen encoder-press shortcut when no workout is active. Same screen, not a
  separate variant.

**Components on screen:**
- Top bar: Standard (time/date, BT status, battery).
- "Now Playing" info block: current track title + artist. Fixed, non-interactive —
  no left-edge arrow indicator, not part of the scrollable list.
- Scrollable list, top to bottom: **Repeat** row (`Repeat.png` / `Repeat_selected.png`),
  **Shuffle** row (`Shuffle.png` / `Shuffle_selected.png`), then the upcoming tracks in
  queue order (title + artist per row, left-edge arrow indicator).
- Selection highlight: magenta pill outline around whichever row (Repeat, Shuffle, or
  a track) is currently highlighted.
- Scrollbar on the right edge with up/down boundary arrows, sized to the scrollable
  region only (excludes the Now Playing block).

**Encoder rotate (CW/CCW):** Windowed-list-with-pinned-edge scroll: the highlight pill
moves between the visible slots as the encoder turns; once it reaches the top or
bottom visible slot, it pins there and the underlying list scrolls behind it instead.
Clamps at the real ends of the list — Repeat is the topmost stop, the last queued
track is the bottommost — does **not** wrap.

**Encoder press:**
- On **Repeat** or **Shuffle**: toggles that setting, swapping to its `_selected` art.
- On a **track row**: skips playback directly to that track.

**Other buttons active on this screen:** Hype (left) = Back → retraces entry point
(Home if opened via the Home shortcut, Main Menu if opened via the menu item); Rest
(right) = Info → contextual help overlay. Global music buttons behave as usual.

**Exit points:** Hype/Back → Home or Main Menu (per entry point). Encoder press on a
track jumps playback to that track and stays on this screen.

**Open questions:** none remaining for this screen.

---

### Settings — Main

**Status:** Done

**Purpose:** top-level settings hub, routes to the three configuration branches
(Display, Hype & Rest, Pairing).

**Entry points:** From Main Menu → "Settings" selected → encoder press.

**Components on screen:**
- Top bar: Standard.
- Header ↩ (back) and ? (info) icons (driven by Hype/Rest physical buttons, as usual).
- 3 vertically stacked list items: Display / Hype & Rest / Pairing, each with an icon box.
- Same visual pattern as Main Menu (Pattern C, per `08-lopaka-screens-explained.md`):
  selected item gets a thick magenta pill outline + bright text; unselected items get
  a thin gray outline + dim text; scrollbar arrows at the top/bottom edge rather than a
  moving thumb, since all 3 items fit the visible slots at once (code structure is
  effectively identical to `menu_1/2/3`, just with different labels).

**Encoder rotate:** moves selection between the 3 fixed slots. Clamps at the
boundaries — no wraparound (matches Main Menu).

**Encoder press:** navigates to the selected branch:
- Display → Settings — Display
- Hype & Rest → Settings — Hype & Rest
- Pairing → not yet mocked

**Other buttons active on this screen:** Hype (left) = Back → Main Menu; Rest (right)
= Info → contextual help overlay. Global music buttons behave as usual.

**Exit points:** Hype/Back → Main Menu. Encoder press → respective sub-screen.

**Open questions:** none remaining for this screen.

---

### Settings — Display

**Status:** Done

**Purpose:** sub-hub for display-related settings.

**Entry points:** From Settings — Main → "Display" selected → encoder press.

**Components on screen:**
- Same structure and visual pattern as Settings — Main (Pattern C, 3 fixed slots).
- 3 items: Brightness / Theme / Home screen.

**Encoder rotate:** moves selection between the 3 fixed slots. Clamps at the
boundaries — no wraparound (matches Settings — Main / Main Menu).

**Encoder press:** navigates to the selected item:
- Brightness → Settings — Brightness
- Theme → not yet mocked
- Home screen → not yet mocked (should expose the same combined/music/workout
  home-screen preference described in `02-firmware.md`, mirrored on-device for
  standalone adjustment — confirm this once that sub-screen is actually designed)

**Other buttons active on this screen:** Hype (left) = Back → Settings — Main; Rest
(right) = Info → contextual help overlay. Global music buttons behave as usual.

**Exit points:** Hype/Back → Settings — Main. Encoder press → respective sub-screen
(Brightness is Done; Theme and Home-screen preference are still unmocked).

**Open questions:** none remaining for this screen's own behavior. Theme and the
Home-screen-preference screen still need their own discussion once mocked.

---

### Settings — Brightness

**Status:** Done

**Purpose:** live brightness adjustment with an on-screen legibility preview, so the
user can tune brightness until the reference content is comfortably visible.

**Entry points:** From Settings — Display → "Brightness" selected → encoder press.

**Components on screen:**
- Top bar: Standard.
- Legibility preview block: sample text at two sizes, plus a blank reference box and
  a row of small reference icons (clock, gear, headphones, snowflake, lightning,
  phone) rendered at the current brightness level.
- Instructional caption: "Adjust brightness until visuals above are properly visible."
- Slider bar with a "-" icon on the left and a "+" icon on the right flanking it.

**Encoder rotate:** continuously adjusts brightness in real time — the slider fill
and the preview content above both update live as the encoder turns. The "-" and "+"
icons are directional indicators only (showing which way brightness is currently
moving), not separate selectable stops.

**Encoder press:** no distinct action defined here — brightness changes apply live as
you rotate, so there's nothing separate to "commit." (Reasonable default, flagged in
case a distinct confirm/save step is actually wanted — the only way off this screen
today is Hype/Back.)

**Other buttons active on this screen:** Hype (left) = Back → Settings — Display;
Rest (right) = Info → contextual help overlay. Global music buttons behave as usual.

**Exit points:** Hype/Back → Settings — Display.

**Open questions:** none blocking. Minor assumption flagged above re: encoder press
having no distinct action — confirm or correct during implementation if a save/commit
step turns out to be wanted after all.

---

### Settings — Hype & Rest

**Status:** Done

**Purpose:** per-button (Hype and Rest) configuration of target playlist, timer
duration, and whether that duration is manually set or auto-derived from exercise
data.

**Entry points:** From Settings — Main → "Hype & Rest" selected → encoder press.

**Components on screen:**
Two mirrored blocks, Hype above and Rest below, each containing:
- "Exercise based time" checkbox (tickbox marked/unmarked bitmaps).
- "Time:" row — minute value + "Minute(s)" label, second value + "Second(s)" label,
  each value with its own small up/down indicator icon (selected/unselected state
  per field).
- "Playlist:" row — a pill-styled text field showing the currently assigned
  playlist name (e.g. "Varjish" for Hype, "Super chill" for Rest).

**Field navigation model (applies uniformly to every field, both blocks — checkbox,
minute, second, playlist):**
1. Encoder rotate (no field in edit mode) moves focus between fields — highlight
   shown as whichever field currently has visual focus.
2. Encoder press **enters edit mode** on the focused field.
3. Encoder rotate **while in edit mode** changes that field's value:
   - Checkbox: rotate flips ticked/unticked.
   - Minute / Second: rotate increments/decrements the numeric value.
   - Playlist: rotate cycles through the app-curated shortlist, with the same
     directional pill-nudge-and-spring-back visual used on Choose Split/Choose
     Workout (see the cross-cutting carousel-bounce note above) as the in-edit-mode
     feedback — this is why the field is rendered as a pill, even though the
     screen as a whole is not a carousel-pattern screen.
4. Encoder press again **commits** the value and returns to field-navigation mode
   (focus stays on that field; rotate then moves focus onward as in step 1).
5. Hype/Back while in edit mode cancels without committing and returns to
   field-navigation mode (consistent with the cancel behavior already established
   for Guided Logging's set editing).

**Assumption (not yet explicitly confirmed, low-stakes):** focus order runs
top-to-bottom within a block (Checkbox → Minute → Second → Playlist), Hype block
before Rest block, wrapping from Rest's Playlist back to Hype's Checkbox. Flag if a
different order or a hard stop at each block's boundary is wanted instead.

**"Exercise based time" behavior (resolved):**
- **Ticked:** this block's duration comes from a new per-exercise authored field
  instead of the Minute/Second value shown here — `target hype seconds` for the Hype
  block, `target rest seconds` for the Rest block. Applies symmetrically to both
  blocks.
- **Unticked:** uses the manual Minute/Second value on this screen, as normal.
- **When ticked, that block's Minute and Second fields become greyed/disabled and
  are skipped in the encoder-rotate focus order** — rotate moves directly from
  Checkbox to Playlist. Unticking restores them to the normal focus order.
- This is an **opt-in exception to the fixed-duration model**, not a change to the
  manual-trigger rule — Hype/Rest are still only triggered by physically pressing
  the button; only the *duration once triggered* can now come from this per-exercise
  field instead of the setting shown here.

**Data/doc consequences (deferred — not blocking sign-off on this screen, but real
follow-up work):**
- `03-workout-logging.md` needs two new fields added to the per-exercise template
  data (and the BLE sync/template schema): `target rest seconds` and
  `target hype seconds`, authored in the app alongside weight/rep targets.
- `02-music-control.md`'s Hype/Rest section needs a short addendum noting this
  opt-in auto-duration exception exists, and that it doesn't change the
  manual-press-only trigger rule.

**Other buttons active on this screen:** Hype (left) = Back → Settings — Main; Rest
(right) = Info → contextual help overlay — this screen is itself one of the
menu/settings-type screens where the physical Hype/Rest buttons are repurposed per
the general cross-cutting rule, even though the screen's *content* configures
Hype/Rest — no conflict, screen context always wins. Global music buttons behave as
usual.

**Exit points:** Hype/Back (outside edit mode) → Settings — Main. Hype/Back while a
field is in edit mode cancels the edit instead of leaving the screen (see step 5
above).

**Open questions:** none remaining for this screen's own behavior, aside from the
low-stakes focus-order assumption flagged above.

---


## Flagged for other docs

All of the doc changes flagged during the screen discussions have now been applied
(2026-09-05). Kept here as a record of what changed and where:

- **`02-music-control.md`** — ✅ done: album art on Home is now documented as real synced
  art (supersedes the "fixed local icons only" lean); format deferred to Phase 2.
- **`02-music-control.md`** — ✅ done: Music Queue screen rewritten so shuffle/repeat are
  described as two rows in the same scrollable list as the queue tracks (one
  list-selection input path), not a separate static toggle UI.
- **`02-music-control.md`** — ✅ done: added the opt-in **"Exercise based time"** addendum
  to the Hype/Rest section (duration can come from a per-exercise field; does not change
  the manual-press-only trigger rule).
- **`03-workout-logging.md`** — ✅ done: added `target hype seconds` and
  `target rest seconds` to the per-exercise cached-template data.
- **`01-ble-communication.md`** — ✅ done: album art over BLE resolved as **yes**;
  now-playing metadata entry and the open-questions list both updated (format —
  raw bitmap vs. JPEG — remains the only open part, tagged for Phase 2).

---

*(Template for remaining screens omitted here for brevity — reuse the structure
above: Status / Purpose / Entry points / Components / Encoder rotate / Encoder
press / Other buttons / Exit points / Open questions.)*
