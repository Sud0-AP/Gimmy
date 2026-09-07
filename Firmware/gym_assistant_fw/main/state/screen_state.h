/*
 * screen_state.h - Central state structs for all UI screens
 *
 * Single source of truth: the app_logic_task owns one instance of app_state_t.
 * Every other task sees immutable snapshots (render_queue) or sends events
 * (input_event_queue). No shared memory, no mutexes.
 *
 * Screen set is grounded in the actual Lopaka screen exports under
 * Hardware/Display/Lopaka Screens. The implementation plan's 14-screen enum has been
 * extended with SCREEN_WORKOUT_OVERVIEW (log_workout_3, the exercise-list
 * preview between "Choose Workout" and the guided session) and
 * SCREEN_MUSIC_QUEUE (opened by the main menu's "Music Queue" item):
 *    SCREEN_MAIN_MENU                     menu_1/2/3
 *    SCREEN_TEMPLATE_LIST (Choose Split)  log_workout_1
 *    SCREEN_EXERCISE_LIST (Choose Workout) log_workout_2
 *    SCREEN_WORKOUT_OVERVIEW              log_workout_3
 *    SCREEN_WORKOUT_SESSION (logging)     log_workout_4
 *    SCREEN_EXERCISE_STATS                log_workout_5
 *    SCREEN_MUSIC_QUEUE                   music_queue
 *    SCREEN_SETTINGS_*                    settings_main/display/hype_rest
 */

#ifndef SCREEN_STATE_H
#define SCREEN_STATE_H

#include <stdbool.h>
#include <stdint.h>

#include "app_config.h"

/* ============================================================================
 * Screen identifiers
 * ============================================================================ */

typedef enum {
    SCREEN_NONE               = 0,

    /* Home screen variants */
    SCREEN_HOME_IDLE,             /* Home_screen_both_1 */
    SCREEN_HOME_HYPE_ACTIVE,      /* Hype timer running */
    SCREEN_HOME_REST_ACTIVE,      /* Rest timer running */

    /* Navigation / workout flow */
    SCREEN_MAIN_MENU,             /* menu_1/2/3: Workout Logs / Music Queue / Settings */
    SCREEN_TEMPLATE_LIST,         /* log_workout_1: Choose Split (Push Pull Legs, ...) */
    SCREEN_EXERCISE_LIST,         /* log_workout_2: Choose Workout for the split */
    SCREEN_WORKOUT_OVERVIEW,      /* log_workout_3: exercises + Start button */
    SCREEN_WORKOUT_SESSION,       /* log_workout_4: guided set logging */
    SCREEN_WORKOUT_COMPLETE,      /* end-of-workout summary */
    SCREEN_EXERCISE_STATS,        /* log_workout_5: last session / personal best */

    /* Music */
    SCREEN_MUSIC_QUEUE,           /* music_queue: Now Playing + queue track list */

    /* Settings */
    SCREEN_SETTINGS_MAIN,         /* settings_main: Display / Hype & Rest / Pairing */
    SCREEN_SETTINGS_DISPLAY,      /* settings_display: Brightness / Home screen / Theme */
    SCREEN_SETTINGS_HYPE_REST,    /* settings_hype_rest: Hype time / Rest seconds / playlists */
    SCREEN_SETTINGS_PAIRING,      /* BLE pairing (stub, not yet exported) */

    /* Stats / history / info */
    SCREEN_STATS_OVERVIEW,        /* totals, weekly volume, best lift */
    SCREEN_HISTORY_LIST,          /* recent sessions */
    SCREEN_INFO,                  /* header Back/Info overlay */

    SCREEN_COUNT                  /* sentinel - number of screens */
} screen_id_t;

/* ============================================================================
 * Generic list screen state
 * ============================================================================ */

/*
 * A single row in any list screen. Pointers point at flash-resident dummy data
 * (or at items_buf for generated lists); never heap-allocated.
 *
 * subtitle is optional (NULL / "") - used for exercise target-muscle captions,
 * queue artists, and history summaries. log_workout_3 shows these as secondary
 * text rows under each exercise name.
 */
typedef struct {
    const char *title;
    const char *subtitle;
} list_item_t;

typedef struct {
    const list_item_t *items;   /* static dummy table, OR = items_buf when generated */
    int count;                  /* number of rows */
    int selected_index;         /* 0-based, only one always selected */

    /* backing store for lists built at init time (workout picker, overview) */
    list_item_t items_buf[16];

    /* Screen frame/header caption (e.g. "Choose Split:", "Push Pull Legs") */
    const char *header;
} list_screen_state_t;

/* ============================================================================
 * Workout data model
 *
 * Static (flash) shape: workout_template_t -> workout_plan_t ->
 * workout_exercise_t -> workout_set_t. The mutable workout_session_state_t
 * points into the active plan and tracks where the user is.
 * ============================================================================ */

/* One set row in the guided-logging table (log_workout_4):
 *   Warmup 1 | 15 Kg | 12-15
 *   Warmup 2 | 20 Kg | 12-15
 *   Work Set 1| 20 Kg | 6-8 ... */
typedef struct {
    char  label[14];    /* "Warmup 1", "Work Set 1" */
    int   weight_kg;
    char  reps[8];      /* target reps, may be a range "12-15" */
} workout_set_t;

/* One exercise with its set table and target-muscle caption (log_workout_3/4). */
typedef struct {
    const char *name;               /* "Incline Dumbbell Press" */
    const char *muscles;            /* "Chest, front delts" */
    int   set_count;
    const workout_set_t *sets;
} workout_exercise_t;

/* A named workout inside a split (e.g. "Push A" inside "Push Pull Legs"). */
typedef struct {
    const char *name;
    int  exercise_count;
    const workout_exercise_t *const *exercises;   /* array of ptrs into exercise pool */
} workout_plan_t;

/* A training split / template: a flat index into the workout registry. */
typedef struct {
    const char *name;
    int  workout_start;   /* flat index of first workout in g_workout_plans */
    int  workout_count;
} workout_template_t;

/*
 * Mutable in-progress session. Keeps the plan's scalar fields
 * (exercise_name/current_set/total_sets/weight_kg/target_reps/completed_reps)
 * so Phase 5's handle_workout_session_input works as designed, plus the flat
 * plan pointer + exercise_index so the guided-logging screen can step through
 * the workout's exercises (Next/Previous buttons on log_workout_4).
 */
typedef struct {
    const workout_plan_t *plan;    /* active plan, NULL until session starts */
    int  exercise_index;           /* index into plan->exercises */

    char exercise_name[32];
    int  current_set;              /* 1-based working set in progress (1..total_sets) */
    int  total_sets;               /* working sets for the active exercise */
    int  weight_kg;                /* weight being adjusted (5kg increments) */
    int  target_reps;
    int  completed_reps;
} workout_session_state_t;

/* End-of-workout summary (draw_workout_complete / SCREEN_WORKOUT_COMPLETE). */
typedef struct {
    int  sets_completed;
    int  exercises_completed;
    int  total_volume_kg;
    int  duration_min;
    const char *strength_score;    /* e.g. "B+" */
} workout_stats_t;

/* ============================================================================
 * Timer & music state
 * ============================================================================ */

/*
 * Hype/Rest timer. timer_seconds counts down while running; total_seconds is
 * the configured duration - the home screen progress bar is
 *   bar_width = 280 * timer_seconds / total_seconds
 * (per Phase 4 render spec).
 */
typedef struct {
    int  timer_seconds;    /* seconds remaining */
    int  total_seconds;    /* full duration, for the progress bar */
    bool is_running;
    bool is_hype;          /* true = Hype, false = Rest */
} timer_state_t;

/*
 * Current playback, shown on the home screen and music queue. queue list
 * itself lives in dummy_data (g_music_queue); this carries the Now Playing
 * track + transport state. BLE is a stub in these phases - toggling is_playing
 * only flips state and logs.
 */
typedef struct {
    const char *song_title;
    const char *artist;
    bool is_playing;
    bool shuffle;
    bool repeat;
    int  progress_seconds;
    int  duration_seconds;
} spotify_state_t;

/* ============================================================================
 * Stats, history, and per-exercise stats
 * ============================================================================ */

/* Aggregate totals for SCREEN_STATS_OVERVIEW. */
typedef struct {
    int  total_workouts;
    int  total_sets_logged;
    int  total_volume_kg;
    int  weekly_workouts;      /* this week */
    int  weekly_minutes;
    const char *best_lift_name;
    int  best_lift_weight_kg;
    const char *best_lift_date;
} stats_data_t;

/* Per-exercise last-session / personal-best for SCREEN_EXERCISE_STATS. */
typedef struct {
    char exercise_name[32];

    /* Last session (log_workout_5 notes section) */
    char last_session_notes[40];      /* "30 degree incline" */
    char last_session_date[16];       /* "15/4/2025" */
    int  last_session_top_weight_kg;
    int  last_session_volume_kg;

    /* Personal best */
    char pr_date[16];                 /* "1/7/2025" */
    int  pr_weight_kg;
    int  pr_reps;
} exercise_stats_t;

/* ============================================================================
 * Top-level application state
 * ============================================================================ */

typedef struct {
    screen_id_t          current_screen;
    screen_id_t          previous_screen;   /* for Back navigation */

    list_screen_state_t  list_state;        /* active list for any list screen */
    workout_session_state_t workout;        /* in-progress session */
    workout_stats_t      workout_stats;     /* last completed session summary */
    timer_state_t        timer;
    spotify_state_t      spotify;
    stats_data_t         stats_overview;
    exercise_stats_t     exercise_stats;
} app_state_t;

#endif // SCREEN_STATE_H