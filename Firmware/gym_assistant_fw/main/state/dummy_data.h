/*
 * dummy_data.h - Mock data + init helpers for the Phase 2 UI (pre-BLE)
 *
 * In production these tables come from the phone app over BLE and are cached in
 * flash by storage_task; for now they are static const tables compiled into
 * flash. Every init_*() populates the list/state struct that the render and
 * state-machine layers consume, so swapping in real data later requires no
 * refactoring of those layers (see plan: "Integration Points for Real Data").
 *
 * Data is grounded in the actual Lopaka screen exports (see screen_state.h for
 * the mapping) rather than the implementation plan's placeholder labels.
 */

#ifndef DUMMY_DATA_H
#define DUMMY_DATA_H

#include "screen_state.h"
#include "input_events.h"

/* ----------------------------------------------------------------------------
 * Workout registry accessors
 *
 * Workouts live in one flat registry (index 0..workout_get_count()-1), sliced
 * per split/template. The workout picker screen is populated via
 * init_exercise_list with a *template* index; the overview and session init
 * take a flat *workout* index:
 *     flat = workout_template_workout_start(template_index) + list.selected_index
 * -------------------------------------------------------------------------- */

int workout_get_count(void);                                        /* all workouts, all splits */
const workout_template_t *workout_get_template(int template_index); /* NULL if out of range */
int workout_template_workout_start(int template_index);             /* flat index of first workout */
const workout_plan_t *workout_get_plan(int workout_index);          /* NULL if out of range */

/* ----------------------------------------------------------------------------
 * Screen list initializers
 * -------------------------------------------------------------------------- */

/* Main menu: Workout Logs / Music Queue / Settings (Lopaka menu_1/2/3) */
void init_main_menu(list_screen_state_t *list);

/* Choose Split (log_workout_1): Push Pull Legs / Upper Lower Arms / ... */
void init_template_list(list_screen_state_t *list);

/*
 * Choose Workout (log_workout_2) for the given split/template.
 * template_index is into workout_get_template(); rows are the split's workouts
 * (Push A / Pull A / ...). list->header carries the split name.
 */
void init_exercise_list(list_screen_state_t *list, int template_index);

/*
 * Workout overview (log_workout_3): the plan's exercises with target-muscle
 * captions + Start button. workout_index is the flat registry index.
 */
void init_workout_overview(list_screen_state_t *list, int workout_index);

/* Guided logging (log_workout_4): prepare the session for the given workout */
void init_workout_session(workout_session_state_t *workout, int workout_index);

/* Settings (settings_main / settings_display / settings_hype_rest) */
void init_settings_menu(list_screen_state_t *list);
void init_settings_display(list_screen_state_t *list);
void init_settings_hype_rest(list_screen_state_t *list);

/* Music queue (music_queue): Now Playing header + track list */
void init_music_queue(list_screen_state_t *list);

/* History list: recent sessions with dates/duration */
void init_history_list(list_screen_state_t *list);

/* ----------------------------------------------------------------------------
 * App state
 *
 * Seeds the full app_state_t (home screen, spotify, timer, stats) and the
 * initial list contents for the current screen at boot.
 * -------------------------------------------------------------------------- */

void init_app_state(app_state_t *state);

#endif // DUMMY_DATA_H