# Hardware UI Implementation Plan

## Context

This plan implements the complete hardware UI for the Gym Assistant device using ESP-IDF and FreeRTOS. Currently, basic hardware validation has been completed via Arduino bring-up sketches (display, encoder, PCF8574T buttons all confirmed working). This plan transitions from Arduino prototypes to production ESP-IDF firmware with a functional UI using dummy data, preparing the architecture for real data integration later.

The device features a 280×240 landscape LCD, rotary encoder, and 5 buttons via I2C expander. All UI screens have been designed in Lopaka and exported as static code blocks that need to be converted into parametric, state-driven rendering functions.

## Project Structure

```
Firmware/
├── main/
│   ├── main.c                          # ESP-IDF app entry, task creation
│   ├── app_config.h                    # Pin definitions, I2C address, task priorities
│   ├── tasks/
│   │   ├── display_task.c/h            # Display rendering, animations (24 FPS)
│   │   ├── input_task.c/h              # Encoder + PCF8574T polling
│   │   ├── app_logic_task.c/h          # State machine, timer logic
│   │   ├── ble_task.c/h                # Stub for Phase 1 (placeholder only)
│   │   └── storage_task.c/h            # Stub for Phase 1 (placeholder only)
│   ├── state/
│   │   ├── screen_state.h              # State structs for all screens
│   │   ├── input_events.h              # Input event enum + queue types
│   │   └── dummy_data.c/h              # Mock workout templates, songs, history
│   ├── ui/
│   │   ├── screens/
│   │   │   ├── home_screen.c/h         # Parametric draw_home_*() functions
│   │   │   ├── workout_screen.c/h      # draw_workout_session(), draw_workout_complete()
│   │   │   ├── template_screen.c/h     # draw_template_list()
│   │   │   ├── exercise_screen.c/h     # draw_exercise_list()
│   │   │   ├── settings_screen.c/h     # draw_settings_*()
│   │   │   ├── stats_screen.c/h        # draw_stats_*(), draw_history_list()
│   │   │   └── info_screen.c/h         # draw_info_overlay()
│   │   ├── animations.c/h              # Animation frame arrays, playback logic
│   │   └── ui_common.c/h               # Shared drawing helpers (scrollbar, pills)
│   └── drivers/
│       ├── lcd/                        # Copy of Waveshare LCD_1.83_Code/Pico/ driver
│       │   ├── Config.c/h              # SPI initialization
│       │   ├── LCD_Driver.c/h          # NV3030B controller driver
│       │   ├── GUI_Paint.c/h           # 2D primitives
│       │   ├── font12.c/font24.c       # Bitmap fonts
│       │   └── tft_compat.h            # TFT_eSPI shim (copy from Unit_Tests)
│       ├── encoder.c/h                 # GPIO interrupt + direction decode
│       └── pcf8574.c/h                 # I2C button expander driver
├── CMakeLists.txt                      # ESP-IDF build config
└── sdkconfig                           # ESP32S3 target, FreeRTOS config
```

## Implementation Phases

### Phase 1: Project Scaffolding & Driver Integration
**Goal:** ESP-IDF project compiles and initializes hardware peripherals.

**Tasks:**
1. Create `Firmware/` directory with ESP-IDF boilerplate (`idf.py create-project`)
2. Copy Waveshare LCD driver from `Hardware/Display/LCD_1.83_Code/Pico/` to `Firmware/main/drivers/lcd/`
3. Copy `Hardware/Display/TFT_Compat_Library/tft_compat.h` to `Firmware/main/drivers/lcd/`
4. Port Arduino Wire calls to ESP-IDF I2C driver API in `pcf8574.c`
5. Port Arduino `attachInterrupt` to ESP-IDF GPIO ISR API in `encoder.c`
6. Create `app_config.h` with confirmed pinout:
   - LCD: CS=D0(GPIO1), DC=D1(GPIO2), RST=D3(GPIO4), MOSI=D10(GPIO9), SCK=D8(GPIO7)
   - Encoder: CLK=D6(GPIO43), DT=D7(GPIO44)
   - I2C: SDA=D4(GPIO5), SCL=D5(GPIO6), PCF8574T address=0x20
7. Create `main.c` with basic initialization (no tasks yet) and verify hardware init succeeds

**Verification:** `idf.py build` succeeds, `idf.py flash monitor` shows "Display initialized" and "I2C initialized" log messages.

---

### Phase 2: State Definitions & Dummy Data
**Goal:** Define all state structs and populate with realistic mock data.

**Tasks:**
1. Create `state/screen_state.h` with structs for all 13 screens:
   ```c
   typedef enum {
     SCREEN_HOME_IDLE,
     SCREEN_HOME_HYPE_ACTIVE,
     SCREEN_HOME_REST_ACTIVE,
     SCREEN_MAIN_MENU,
     SCREEN_TEMPLATE_LIST,
     SCREEN_EXERCISE_LIST,
     SCREEN_WORKOUT_SESSION,
     SCREEN_WORKOUT_COMPLETE,
     SCREEN_SETTINGS_MAIN,
     SCREEN_SETTINGS_WORKOUT,
     SCREEN_SETTINGS_MUSIC,
     SCREEN_STATS_OVERVIEW,
     SCREEN_HISTORY_LIST,
     SCREEN_INFO
   } screen_id_t;

   typedef struct {
     int selected_index;
     int list_length;
     char **list_items;  // Pointer to dummy data array
   } list_screen_state_t;

   typedef struct {
     char exercise_name[32];
     int current_set;
     int total_sets;
     int weight_kg;
     int target_reps;
     int completed_reps;
   } workout_session_state_t;

   typedef struct {
     int timer_seconds;
     bool is_running;
     bool is_hype;  // true = Hype, false = Rest
   } timer_state_t;

   typedef struct {
     char song_title[48];
     char artist[32];
     bool is_playing;
   } spotify_state_t;

   typedef struct {
     screen_id_t current_screen;
     screen_id_t previous_screen;  // For Back navigation
     list_screen_state_t list_state;
     workout_session_state_t workout;
     timer_state_t timer;
     spotify_state_t spotify;
   } app_state_t;
   ```

2. Create `state/input_events.h`:
   ```c
   typedef enum {
     INPUT_ENCODER_CW,       // Clockwise rotation
     INPUT_ENCODER_CCW,      // Counter-clockwise rotation
     INPUT_ENCODER_PUSH,
     INPUT_BTN_PREV,
     INPUT_BTN_PLAY,
     INPUT_BTN_NEXT,
     INPUT_BTN_HYPE,
     INPUT_BTN_REST
   } input_event_t;
   ```

3. Create `state/dummy_data.c/h` with mock data:
   - **Main menu items**: "Start Workout", "Settings", "Stats", "History"
   - 5 workout templates: "Push Day", "Pull Day", "Leg Day", "Upper Body", "Full Body"
   - 10 exercises: "Bench Press", "Squat", "Deadlift", "Overhead Press", etc.
   - 3 settings categories with 3-4 options each
   - 5 workout history entries with dates and stats
   - Mock Spotify song: "Eye of the Tiger - Survivor"
   - Initialize with realistic values (e.g., workout session: Bench Press, Set 2/3, 80kg, 5 reps)

**Verification:** Code compiles, dummy data arrays accessible from other modules.

---

### Phase 3: FreeRTOS Task Infrastructure
**Goal:** Create 5 FreeRTOS tasks with queue communication.

**Tasks:**
1. Create task files with stub implementations:
   - `tasks/display_task.c/h` - priority 5 (high), 4KB stack
   - `tasks/input_task.c/h` - priority 5 (high), 2KB stack
   - `tasks/app_logic_task.c/h` - priority 3 (medium), 4KB stack
   - `tasks/ble_task.c/h` - priority 3 (medium), 4KB stack (stub only, logs "BLE not implemented")
   - `tasks/storage_task.c/h` - priority 1 (low), 2KB stack (stub only)

2. Create queue handles in `main.c`:
   ```c
   QueueHandle_t input_event_queue;   // input_task → app_logic_task
   QueueHandle_t render_queue;        // app_logic_task → display_task
   ```

3. Implement `input_task`:
   - Initialize encoder GPIO interrupts
   - Initialize I2C for PCF8574T
   - Poll I2C at 20Hz (vTaskDelay(50ms))
   - Debounce button presses (simple: track previous state, require 2 consecutive reads)
   - Read encoder position delta from ISR volatile variable
   - Send `input_event_t` to `input_event_queue`

4. Implement `app_logic_task` skeleton:
   - Initialize `app_state_t` with dummy data (start at SCREEN_HOME_IDLE)
   - Wait on `input_event_queue` with 100ms timeout
   - Log received events to console (full state machine implementation in Phase 5)
   - Send render command to `render_queue` (just current screen_id for now)

5. Implement `display_task` skeleton:
   - Initialize LCD hardware (Config_Init, LCD_Init, Paint_NewImage)
   - Wait on `render_queue`
   - Log "Rendering screen: <id>" (actual rendering in Phase 4)
   - Call `LCD_Display(ImageBuf)` with blank buffer

6. Update `main.c`:
   - Create queues
   - Spawn all 5 tasks
   - Call `vTaskStartScheduler()`

**Verification:** Flash and monitor logs show:
- "input_task started"
- "app_logic_task started"
- "display_task started"
- Rotating encoder logs "INPUT_ENCODER_CW" / "INPUT_ENCODER_CCW"
- Pressing buttons logs "INPUT_BTN_PLAY", etc.

---

### Phase 4: UI Screen Rendering Functions
**Goal:** Convert all 13 Lopaka screens to parametric draw functions.

**Reuse from Arduino sketch:** `Hardware/Unit_Tests/Display_Encoder_Test/LCD_1in83/` already demonstrates dynamic selection rendering and scrollbar calculation - adapt this pattern.

**Tasks:**
1. Create `ui/ui_common.c/h` with shared helpers:
   ```c
   void draw_scrollbar(int selected_index, int total_items, int base_y, int height);
   void draw_pill(int x, int y, int w, int h, bool selected);  // Filled if selected
   void draw_header_button(int x, int y, int radius, bool selected);  // Back/Info ellipses
   ```

2. Create `ui/screens/home_screen.c/h`:
   - `draw_home_idle(spotify_state_t *spotify)`
   - `draw_home_hype_active(timer_state_t *timer, spotify_state_t *spotify)`
   - `draw_home_rest_active(timer_state_t *timer, spotify_state_t *spotify)`
   - Paste Lopaka blocks from `home_idle.txt`, `home_active_hype.txt`, `home_active_rest.txt`
   - Replace static text with `sprintf()` + state fields
   - Add timer progress bar calculation: `bar_width = (280 * timer->timer_seconds) / MAX_TIMER_SECONDS`

3. Create `ui/screens/workout_screen.c/h`:
   - `draw_workout_session(workout_session_state_t *workout)`
   - `draw_workout_complete(workout_stats_t *stats)`
   - Paste from `workout_session.txt`, `workout_complete.txt`
   - Parametrize exercise name, set counts, weight/rep values

4. Create `ui/screens/template_screen.c/h`:
   - `draw_template_list(list_screen_state_t *list)`
   - `draw_main_menu(list_screen_state_t *list)` - similar to template list but with menu items
   - Paste from `template_list.txt`
   - Loop over `list->list_items[0..list_length-1]`
   - Apply selection highlighting: `if (i == list->selected_index) use 0xE33F`
   - Call `draw_scrollbar(list->selected_index, list->list_length, ...)`

5. Create `ui/screens/exercise_screen.c/h`:
   - `draw_exercise_list(list_screen_state_t *list)` - identical pattern to template_list

6. Create `ui/screens/settings_screen.c/h`:
   - `draw_settings_main(list_screen_state_t *list)`
   - `draw_settings_workout(list_screen_state_t *list)`
   - `draw_settings_music(list_screen_state_t *list)`
   - All use same list rendering pattern

7. Create `ui/screens/stats_screen.c/h`:
   - `draw_stats_overview(stats_data_t *stats)` - from `stats_overview.txt`
   - `draw_history_list(list_screen_state_t *list)` - from `history_list.txt`

8. Create `ui/screens/info_screen.c/h`:
   - `draw_info_overlay(const char *help_text)` - from `info_screen.txt`

9. Update `display_task.c` to call appropriate draw function:
   ```c
   void render_screen(app_state_t *state) {
     Paint_Clear(0x0801);  // Background color
     switch (state->current_screen) {
       case SCREEN_HOME_IDLE:
         draw_home_idle(&state->spotify);
         break;
       case SCREEN_MAIN_MENU:
         draw_main_menu(&state->list_state);
         break;
       case SCREEN_TEMPLATE_LIST:
         draw_template_list(&state->list_state);
         break;
       // ... all 13 cases
     }
     LCD_Display(ImageBuf);
   }
   ```

10. Modify `render_queue` to pass entire `app_state_t` snapshot (not just screen_id)

**Verification:** Flash firmware, device boots to home screen showing:
- "Eye of the Tiger" song title
- Play/Pause/Next buttons rendered
- Hype/Rest buttons visible

---

### Phase 5: State Machine & Input Handling
**Goal:** Implement per-screen input behavior per 07-ui-interaction-spec.md.

**Tasks:**
1. Implement `app_logic_task` state machine in `tasks/app_logic_task.c`:
   ```c
   void process_input_event(app_state_t *state, input_event_t event) {
     switch (state->current_screen) {
       case SCREEN_HOME_IDLE:
         handle_home_idle_input(state, event);
         break;
       case SCREEN_MAIN_MENU:
       case SCREEN_TEMPLATE_LIST:
       case SCREEN_SETTINGS_MAIN:
         handle_list_input(state, event);  // Generic for all list screens
         break;
       // ... all 13 cases
     }
   }

   void handle_home_idle_input(app_state_t *state, input_event_t event) {
     switch (event) {
       case INPUT_ENCODER_PUSH:
         state->previous_screen = SCREEN_HOME_IDLE;
         state->current_screen = SCREEN_MAIN_MENU;
         init_main_menu(&state->list_state);  // Load main menu items
         break;
       case INPUT_BTN_HYPE:
         state->timer.is_hype = true;
         state->timer.timer_seconds = 180;  // 3 min default
         state->timer.is_running = true;
         state->current_screen = SCREEN_HOME_HYPE_ACTIVE;
         break;
       case INPUT_BTN_REST:
         state->timer.is_hype = false;
         state->timer.timer_seconds = 90;  // 90 sec default
         state->timer.is_running = true;
         state->current_screen = SCREEN_HOME_REST_ACTIVE;
         break;
       case INPUT_BTN_PLAY:
         state->spotify.is_playing = !state->spotify.is_playing;
         // TODO Phase 2: send BLE command
         break;
       // ... other buttons
     }
   }

   void handle_list_input(app_state_t *state, input_event_t event) {
     switch (event) {
       case INPUT_ENCODER_CW:
         if (state->list_state.selected_index < state->list_state.list_length - 1)
           state->list_state.selected_index++;
         break;
       case INPUT_ENCODER_CCW:
         if (state->list_state.selected_index > 0)
           state->list_state.selected_index--;
         break;
       case INPUT_ENCODER_PUSH:
         // Screen-specific selection handler
         handle_list_selection(state);
         break;
       case INPUT_BTN_HYPE:  // Left button = Back in menus
         state->current_screen = state->previous_screen;
         break;
       case INPUT_BTN_REST:  // Right button = Info in menus
         state->previous_screen = state->current_screen;
         state->current_screen = SCREEN_INFO;
         break;
     }
   }

   void handle_list_selection(app_state_t *state) {
     switch (state->current_screen) {
       case SCREEN_MAIN_MENU:
         // Main menu: "Start Workout", "Settings", "Stats", "History"
         switch (state->list_state.selected_index) {
           case 0:  // Start Workout
             state->previous_screen = SCREEN_MAIN_MENU;
             state->current_screen = SCREEN_TEMPLATE_LIST;
             init_template_list(&state->list_state);
             break;
           case 1:  // Settings
             state->previous_screen = SCREEN_MAIN_MENU;
             state->current_screen = SCREEN_SETTINGS_MAIN;
             init_settings_menu(&state->list_state);
             break;
           // ... Stats, History
         }
         break;
       case SCREEN_TEMPLATE_LIST:
         // Selected template, load exercises for that template
         state->previous_screen = SCREEN_TEMPLATE_LIST;
         state->current_screen = SCREEN_EXERCISE_LIST;
         init_exercise_list(&state->list_state, state->list_state.selected_index);
         break;
       case SCREEN_EXERCISE_LIST:
         // Selected exercise, start workout session
         state->current_screen = SCREEN_WORKOUT_SESSION;
         init_workout_session(&state->workout, state->list_state.selected_index);
         break;
       // ... other list screen transitions
     }
   }
   ```

2. Implement workout session input handling:
   ```c
   void handle_workout_session_input(app_state_t *state, input_event_t event) {
     switch (event) {
       case INPUT_ENCODER_CW:
         state->workout.weight_kg += 5;  // 5kg increments
         break;
       case INPUT_ENCODER_CCW:
         if (state->workout.weight_kg >= 5)
           state->workout.weight_kg -= 5;
         break;
       case INPUT_ENCODER_PUSH:
         // Log set, advance to next
         state->workout.completed_reps = state->workout.target_reps;
         state->workout.current_set++;
         if (state->workout.current_set > state->workout.total_sets) {
           state->current_screen = SCREEN_WORKOUT_COMPLETE;
           calculate_workout_stats(&state->workout_stats);
         }
         break;
       case INPUT_BTN_HYPE:
         start_timer(state, true);  // Hype timer
         break;
       case INPUT_BTN_REST:
         start_timer(state, false);  // Rest timer
         break;
     }
   }
   ```

3. Implement timer countdown logic in `app_logic_task` main loop:
   ```c
   void app_logic_task(void *pvParameters) {
     app_state_t state;
     init_app_state(&state);  // Load dummy data
     
     TickType_t last_timer_update = xTaskGetTickCount();
     
     while (1) {
       input_event_t event;
       if (xQueueReceive(input_event_queue, &event, pdMS_TO_TICKS(100))) {
         process_input_event(&state, event);
         send_render_command(&state);  // Queue full state snapshot to display_task
       }
       
       // Timer countdown (1 Hz)
       TickType_t now = xTaskGetTickCount();
       if (state.timer.is_running && (now - last_timer_update >= pdMS_TO_TICKS(1000))) {
         state.timer.timer_seconds--;
         if (state.timer.timer_seconds <= 0) {
           state.timer.is_running = false;
           // Return to appropriate screen
           if (state.current_screen == SCREEN_HOME_HYPE_ACTIVE ||
               state.current_screen == SCREEN_HOME_REST_ACTIVE) {
             state.current_screen = SCREEN_HOME_IDLE;
           }
         }
         send_render_command(&state);
         last_timer_update = now;
       }
     }
   }
   ```

**Verification:**
- Boot to home, press encoder → main menu appears
- Rotate encoder → scrollbar thumb moves, selection highlights change
- Select "Start Workout" → template list appears
- Select template → exercise list appears
- Select exercise → workout session screen with "Bench Press, Set 1/3, 80kg"
- Rotate encoder → weight adjusts (85kg, 90kg, ...)
- Press encoder → advances to Set 2/3
- Press Hype button on home → timer starts counting down, progress bar animates
- Press Back (Hype button) in main menu → returns to home

---

### Phase 6: Animations & Polish
**Goal:** Add 24 FPS animations for selected items and loading states.

**Tasks:**
1. Convert animation sprite arrays from Lopaka exports:
   - Copy `Hardware/Display/Lopaka Screens/Animations/*.h` to `Firmware/main/ui/animations.c`
   - Wrap in proper C module:
   ```c
   // animations.h
   typedef struct {
     const uint16_t *frames;  // RGB565 frame buffer array
     int frame_count;
     int width;
     int height;
   } animation_t;

   extern const animation_t anim_spinner_32;
   extern const animation_t anim_spinner_48;
   extern const animation_t anim_pulse_64;
   ```

2. Implement animation playback in `display_task.c`:
   ```c
   typedef struct {
     const animation_t *anim;
     int x;
     int y;
     uint32_t start_time_ms;
   } active_animation_t;

   active_animation_t active_animations[4];  // Max 4 concurrent
   int active_anim_count = 0;

   void render_animations() {
     uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
     for (int i = 0; i < active_anim_count; i++) {
       active_animation_t *a = &active_animations[i];
       int frame = ((now - a->start_time_ms) / 42) % a->anim->frame_count;  // 24 FPS
       const uint16_t *frame_buf = &a->anim->frames[frame * a->anim->width * a->anim->height];
       Paint_DrawImage(frame_buf, a->x, a->y, a->anim->width, a->anim->height);
     }
   }

   void render_screen_with_animations(app_state_t *state) {
     Paint_Clear(0x0801);
     
     // Render static screen
     render_screen(state);
     
     // Clear previous animations
     active_anim_count = 0;
     
     // Add animations based on screen state
     if (state->current_screen == SCREEN_TEMPLATE_LIST && state->list_state.selected_index >= 0) {
       // Add pulse animation to selected item
       int item_y = 50 + (state->list_state.selected_index * 30);
       add_animation(&anim_pulse_64, 200, item_y - 32);
     }
     
     if (state->timer.is_running) {
       // Spinner at timer icon location
       add_animation(&anim_spinner_48, 120, 80);
     }
     
     // Render all active animations
     render_animations();
     
     LCD_Display(ImageBuf);
   }
   ```

3. Update all list screen draw functions to reserve space for animation overlays:
   - Replace Lopaka placeholder `fillRect(x, y, 64, 64, 0x0801)` with comment `// Animation rendered by display_task`

4. Implement display refresh rate limiting:
   ```c
   void display_task(void *pvParameters) {
     init_display_hardware();
     
     app_state_t state_snapshot;
     TickType_t last_render = xTaskGetTickCount();
     
     while (1) {
       // Check for state updates
       if (xQueueReceive(render_queue, &state_snapshot, pdMS_TO_TICKS(42))) {  // 24 FPS timeout
         render_screen_with_animations(&state_snapshot);
         last_render = xTaskGetTickCount();
       } else {
         // No state change, but animate if needed
         TickType_t now = xTaskGetTickCount();
         if (active_anim_count > 0 && (now - last_render >= pdMS_TO_TICKS(42))) {
           render_animations();  // Partial redraw (just animation regions)
           LCD_Display(ImageBuf);
           last_render = now;
         }
       }
     }
   }
   ```

**Verification:**
- Selected list items show pulsing animation
- Active timers show spinner animation
- Animations run smoothly at ~24 FPS
- Screen remains responsive to encoder input during animations

---

### Phase 7: Integration Testing & Refinement
**Goal:** End-to-end testing of all UI flows with dummy data.

**Test scenarios:**
1. **Home → Workout flow:**
   - Boot to home
   - Start Hype timer (should count down, show progress bar)
   - Cancel timer (return to home)
   - Press encoder → main menu appears
   - Select "Start Workout" → template list
   - Select template → exercise list
   - Select exercise → workout session
   - Workout session: adjust weight, complete 3 sets
   - Workout complete screen shows dummy stats
   - Dismiss, return to home

2. **Main Menu & Settings navigation:**
   - Boot to home
   - Encoder push → main menu (Start Workout, Settings, Stats, History)
   - Scroll to "Settings" item, encoder push → settings menu
   - Navigate list with encoder
   - Enter submenu (workout settings)
   - Press Back (Hype button) → return to settings main
   - Press Back again → return to main menu
   - Press Back again → return to home

3. **Spotify controls:**
   - Press Play → "is_playing" toggles, button icon changes
   - Press Next/Prev → log events (no BLE action in Phase 1)

4. **Edge cases:**
   - Scroll encoder at list boundaries (should clamp at 0 and list_length-1)
   - Adjust weight below minimum (should clamp at 0 or 5kg minimum)
   - Timer reaches 0 (should auto-return to home)

5. **Performance checks:**
   - Encoder responsiveness (no dropped inputs)
   - Animation smoothness (no frame drops)
   - Button debouncing (no double-triggers)

**Refinements:**
- Adjust encoder sensitivity if needed (multiply delta by factor)
- Tune animation frame rates if 24 FPS is too fast/slow
- Fix any visual glitches (text overlap, misaligned elements)
- Add debug logging for state transitions (enable via `sdkconfig`)

---

## Critical Files to Reference

### Existing Code (reuse patterns from)
- `Hardware/Unit_Tests/Display_Encoder_Test/LCD_1in83/LCD_1in83.ino` — encoder ISR, PCF8574T polling, state machine, list selection rendering
- `Hardware/Display/TFT_Compat_Library/tft_compat.h` — complete display API (copy as-is)
- `Hardware/Display/LCD_1.83_Code/Pico/` — Waveshare LCD driver (port to ESP-IDF)

### Lopaka Screens (convert to parametric functions)
- `Hardware/Display/Lopaka Screens/*.txt` — all 13 static screen code blocks
- `Hardware/Display/Lopaka Screens/Animations/*.h` — sprite arrays for 24 FPS playback

### Context Documents (behavior specifications)
- `context_documents/01-device-hardware.md` — pinout, I2C address
- `context_documents/02-firmware.md` — FreeRTOS task breakdown
- `context_documents/04-ui-ux.md` — design tokens, screen inventory
- `context_documents/06-display-ui-library.md` — rendering cycle, animation rules
- `context_documents/07-ui-interaction-spec.md` — per-screen encoder/button behavior (GATED SPEC - confirm before coding)
- `context_documents/08-lopaka-screens-explained.md` — static→parametric conversion guide

## Data Flow Architecture

```
Encoder GPIO ISR → volatile encoder_pos
                       ↓
input_task: polls I2C (20Hz), reads encoder_pos delta
                       ↓
            input_event_queue (input_event_t)
                       ↓
app_logic_task: state machine, timer countdown
                       ↓
              render_queue (app_state_t snapshot)
                       ↓
display_task: Paint_Clear → draw_screen() → animations → LCD_Display (24 FPS)
```

**Key principle:** `app_logic_task` owns the single source of truth (`app_state_t`). Other tasks receive immutable snapshots or send events — no shared memory, no mutexes.

## Verification Checklist

After Phase 7 completion, the firmware should:
- ✅ Boot to home screen with dummy Spotify song displayed
- ✅ Navigate through all 13 screens via encoder + buttons
- ✅ Start/cancel Hype and Rest timers with countdown
- ✅ Complete full workout flow (main menu → template → exercise → log sets → completion)
- ✅ Show selection animations on list items
- ✅ Show spinner animations during active timers
- ✅ Respond to encoder rotation within 50ms (no input lag)
- ✅ Back button (Hype) works in all menu screens
- ✅ Info button (Rest) shows info overlay in menu screens
- ✅ Spotify Play/Pause toggles state (logs event, no BLE)
- ✅ All text rendering uses correct fonts (Font12/Font24)
- ✅ Scrollbar thumb position updates dynamically in lists
- ✅ No crashes, no task starvation, no queue overflows

## Integration Points for Real Data (Phase 2+)

The architecture is designed for clean integration later:

1. **BLE Spotify commands:** In `app_logic_task`, replace log statements with queue sends to `ble_task`
2. **Template sync:** `ble_task` receives templates from phone, writes to `storage_task` queue, `app_logic_task` reloads from flash
3. **Workout logging:** On set completion, send workout data to `storage_task` for NVS write, then to `ble_task` for phone sync
4. **Settings persistence:** Settings screen changes write to `storage_task`, loaded on boot

**Current dummy data locations** (replace with real data later):
- `state/dummy_data.c` — templates, exercises, history, songs
- `tasks/app_logic_task.c::init_app_state()` — initial state population

All business logic functions already accept state pointers, so swapping dummy data for real data requires no refactoring of rendering or state machine code.

## Build & Flash Commands

```bash
cd Firmware/
idf.py set-target esp32s3
idf.py menuconfig  # Confirm FreeRTOS tick rate 1000Hz, stack overflow checking
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

## Estimated Complexity

- **Phase 1:** 2-3 hours (ESP-IDF setup, driver porting)
- **Phase 2:** 1-2 hours (state structs, dummy data)
- **Phase 3:** 2-3 hours (FreeRTOS tasks, queues, input polling)
- **Phase 4:** 4-6 hours (13 screen draw functions, Lopaka conversion)
- **Phase 5:** 3-4 hours (state machine, all input handlers)
- **Phase 6:** 2-3 hours (animation playback, 24 FPS loop)
- **Phase 7:** 2-3 hours (testing, refinement)

**Total:** ~16-24 hours of focused development.
