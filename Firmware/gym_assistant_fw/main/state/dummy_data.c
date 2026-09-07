/*
 * dummy_data.c - Mock data tables + init helpers (see dummy_data.h)
 *
 * All workouts live in one flat registry (g_workout_plans) sliced per split by
 * g_templates. Exercises are a shared flash pool (ex_*), referenced by pointer
 * from each workout so set tables are defined once.
 */

#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "dummy_data.h"

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

/* ============================================================================
 * Set tables (one per exercise; weight in kg)
 * ============================================================================ */

static const workout_set_t sets_incline_db_press[] = {
    { "Warmup 1",  15, "12-15" },
    { "Warmup 2",  20, "12-15" },
    { "Work Set 1",20, "6-8"   },
    { "Work Set 2",25, "6-8"   },
    { "Work Set 3",25, "3-4"   },
};
static const workout_set_t sets_pec_deck_fly[] = {
    { "Work Set 1", 30, "12-15" },
    { "Work Set 2", 35, "10-12" },
    { "Work Set 3", 35, "8-10"  },
};
static const workout_set_t sets_cable_pushdown[] = {
    { "Work Set 1", 20, "12-15" },
    { "Work Set 2", 25, "10-12" },
    { "Work Set 3", 25, "8-10"  },
};
static const workout_set_t sets_lateral_raise[] = {
    { "Work Set 1",  8, "12-15" },
    { "Work Set 2", 10, "10-12" },
    { "Work Set 3", 10, "8-10"  },
};
static const workout_set_t sets_overhead_press[] = {
    { "Work Set 1", 20, "8-10" },
    { "Work Set 2", 22, "6-8"  },
    { "Work Set 3", 22, "5-6"  },
    { "Work Set 4", 25, "3-5"  },
};
static const workout_set_t sets_bench_press[] = {
    { "Work Set 1", 40, "8-10" },
    { "Work Set 2", 45, "6-8"  },
    { "Work Set 3", 50, "5-6"  },
    { "Work Set 4", 50, "3-5"  },
};
static const workout_set_t sets_triceps_pushdown[] = {
    { "Work Set 1", 30, "12-15" },
    { "Work Set 2", 35, "10-12" },
    { "Work Set 3", 35, "8-10"  },
};
static const workout_set_t sets_rows[] = {
    { "Work Set 1", 40, "8-10" },
    { "Work Set 2", 45, "6-8"  },
    { "Work Set 3", 50, "6-8"  },
    { "Work Set 4", 50, "5"    },
};
static const workout_set_t sets_lat_pulldown[] = {
    { "Work Set 1", 35, "10-12" },
    { "Work Set 2", 40, "8-10"  },
    { "Work Set 3", 45, "8-10"  },
};
static const workout_set_t sets_face_pull[] = {
    { "Work Set 1", 15, "12-15" },
    { "Work Set 2", 17, "12-15" },
    { "Work Set 3", 17, "10-12" },
};
static const workout_set_t sets_curls[] = {
    { "Work Set 1", 10, "10-12" },
    { "Work Set 2", 12, "8-10"  },
    { "Work Set 3", 12, "8"     },
};
static const workout_set_t sets_squat[] = {
    { "Work Set 1", 60, "8-10" },
    { "Work Set 2", 70, "6-8"  },
    { "Work Set 3", 80, "5-6"  },
    { "Work Set 4", 85, "3-5"  },
};
static const workout_set_t sets_deadlift[] = {
    { "Work Set 1",  60, "5"    },
    { "Work Set 2",  80, "5"    },
    { "Work Set 3", 100, "3-5"  },
    { "Work Set 4", 110, "3"    },
};
static const workout_set_t sets_leg_press[] = {
    { "Work Set 1", 120, "12-15" },
    { "Work Set 2", 140, "10-12" },
    { "Work Set 3", 160, "8-10"  },
};
static const workout_set_t sets_lunges[] = {
    { "Work Set 1", 10, "12-15" },
    { "Work Set 2", 12, "10-12" },
    { "Work Set 3", 14, "8-10"  },
};
static const workout_set_t sets_calf_raise[] = {
    { "Work Set 1", 60, "15-20" },
    { "Work Set 2", 70, "12-15" },
    { "Work Set 3", 70, "10-12" },
};

/* ============================================================================
 * Exercise pool
 * ============================================================================ */

static const workout_exercise_t ex_incline_db_press = {
    .name = "Incline Dumbbell Press", .muscles = "Chest, front delts",
    .set_count = ARRAY_LEN(sets_incline_db_press), .sets = sets_incline_db_press,
};
static const workout_exercise_t ex_pec_deck_fly = {
    .name = "Pec Deck Fly", .muscles = "Chest, shoulders",
    .set_count = ARRAY_LEN(sets_pec_deck_fly), .sets = sets_pec_deck_fly,
};
static const workout_exercise_t ex_cable_pushdown = {
    .name = "Cable Pushdown", .muscles = "Triceps",
    .set_count = ARRAY_LEN(sets_cable_pushdown), .sets = sets_cable_pushdown,
};
static const workout_exercise_t ex_lateral_raise = {
    .name = "Dumbbell Lateral Raise", .muscles = "Side delts, traps",
    .set_count = ARRAY_LEN(sets_lateral_raise), .sets = sets_lateral_raise,
};
static const workout_exercise_t ex_overhead_press = {
    .name = "Overhead Press", .muscles = "Shoulders",
    .set_count = ARRAY_LEN(sets_overhead_press), .sets = sets_overhead_press,
};
static const workout_exercise_t ex_bench_press = {
    .name = "Bench Press", .muscles = "Chest, shoulders, triceps",
    .set_count = ARRAY_LEN(sets_bench_press), .sets = sets_bench_press,
};
static const workout_exercise_t ex_triceps_pushdown = {
    .name = "Triceps Pushdown", .muscles = "Triceps",
    .set_count = ARRAY_LEN(sets_triceps_pushdown), .sets = sets_triceps_pushdown,
};
static const workout_exercise_t ex_rows = {
    .name = "Barbell Rows", .muscles = "Back, biceps",
    .set_count = ARRAY_LEN(sets_rows), .sets = sets_rows,
};
static const workout_exercise_t ex_lat_pulldown = {
    .name = "Lat Pulldown", .muscles = "Back",
    .set_count = ARRAY_LEN(sets_lat_pulldown), .sets = sets_lat_pulldown,
};
static const workout_exercise_t ex_face_pull = {
    .name = "Face Pull", .muscles = "Rear delts, traps",
    .set_count = ARRAY_LEN(sets_face_pull), .sets = sets_face_pull,
};
static const workout_exercise_t ex_curls = {
    .name = "Dumbbell Curls", .muscles = "Biceps",
    .set_count = ARRAY_LEN(sets_curls), .sets = sets_curls,
};
static const workout_exercise_t ex_squat = {
    .name = "Barbell Squat", .muscles = "Quads, glutes",
    .set_count = ARRAY_LEN(sets_squat), .sets = sets_squat,
};
static const workout_exercise_t ex_deadlift = {
    .name = "Deadlift", .muscles = "Posterior chain",
    .set_count = ARRAY_LEN(sets_deadlift), .sets = sets_deadlift,
};
static const workout_exercise_t ex_leg_press = {
    .name = "Leg Press", .muscles = "Quads",
    .set_count = ARRAY_LEN(sets_leg_press), .sets = sets_leg_press,
};
static const workout_exercise_t ex_lunges = {
    .name = "Walking Lunges", .muscles = "Quads, hamstrings",
    .set_count = ARRAY_LEN(sets_lunges), .sets = sets_lunges,
};
static const workout_exercise_t ex_calf_raise = {
    .name = "Standing Calf Raise", .muscles = "Calves",
    .set_count = ARRAY_LEN(sets_calf_raise), .sets = sets_calf_raise,
};

/* ============================================================================
 * Workout definitions (which exercises, in what order)
 * ============================================================================ */

static const workout_exercise_t *const ex_list_push_a[] = {
    &ex_incline_db_press, &ex_pec_deck_fly, &ex_cable_pushdown, &ex_lateral_raise, &ex_overhead_press,
};
static const workout_exercise_t *const ex_list_pull_a[] = {
    &ex_rows, &ex_lat_pulldown, &ex_face_pull, &ex_curls,
};
static const workout_exercise_t *const ex_list_legs_a[] = {
    &ex_squat, &ex_leg_press, &ex_lunges, &ex_calf_raise,
};
static const workout_exercise_t *const ex_list_push_b[] = {
    &ex_bench_press, &ex_triceps_pushdown, &ex_lateral_raise,
};
static const workout_exercise_t *const ex_list_pull_b[] = {
    &ex_deadlift, &ex_rows, &ex_curls,
};
static const workout_exercise_t *const ex_list_upper_a[] = {
    &ex_bench_press, &ex_rows, &ex_lat_pulldown,
};
static const workout_exercise_t *const ex_list_lower_a[] = {
    &ex_squat, &ex_deadlift, &ex_calf_raise,
};
static const workout_exercise_t *const ex_list_arms_a[] = {
    &ex_curls, &ex_triceps_pushdown,
};
static const workout_exercise_t *const ex_list_upper_b[] = {
    &ex_overhead_press, &ex_rows,
};
static const workout_exercise_t *const ex_list_lower_b[] = {
    &ex_squat, &ex_lunges,
};
static const workout_exercise_t *const ex_list_full_body_a[] = {
    &ex_bench_press, &ex_squat, &ex_deadlift, &ex_lateral_raise,
};
static const workout_exercise_t *const ex_list_full_body_b[] = {
    &ex_incline_db_press, &ex_deadlift, &ex_leg_press,
};
static const workout_exercise_t *const ex_list_bro_chest[] = {
    &ex_pec_deck_fly, &ex_bench_press,
};
static const workout_exercise_t *const ex_list_bro_back[] = {
    &ex_lat_pulldown, &ex_rows,
};
static const workout_exercise_t *const ex_list_bro_shoulders[] = {
    &ex_overhead_press, &ex_lateral_raise, &ex_face_pull,
};
static const workout_exercise_t *const ex_list_bro_arms[] = {
    &ex_curls, &ex_triceps_pushdown,
};
static const workout_exercise_t *const ex_list_bro_legs[] = {
    &ex_squat, &ex_leg_press,
};
static const workout_exercise_t *const ex_list_custom_01[] = {
    &ex_bench_press, &ex_curls,
};
static const workout_exercise_t *const ex_list_custom_02[] = {
    &ex_squat, &ex_overhead_press,
};

/* ============================================================================
 * Flat workout registry + split/template slices
 *
 * Index order matters: g_templates.workout_start must match this layout.
 * ============================================================================ */

static const workout_plan_t g_workout_plans[] = {
    { "Push A",      ARRAY_LEN(ex_list_push_a),        ex_list_push_a },
    { "Pull A",      ARRAY_LEN(ex_list_pull_a),        ex_list_pull_a },
    { "Legs A",      ARRAY_LEN(ex_list_legs_a),        ex_list_legs_a },
    { "Push B",      ARRAY_LEN(ex_list_push_b),        ex_list_push_b },
    { "Pull B",      ARRAY_LEN(ex_list_pull_b),        ex_list_pull_b },
    { "Upper A",     ARRAY_LEN(ex_list_upper_a),       ex_list_upper_a },
    { "Lower A",     ARRAY_LEN(ex_list_lower_a),       ex_list_lower_a },
    { "Arms A",      ARRAY_LEN(ex_list_arms_a),        ex_list_arms_a },
    { "Upper B",     ARRAY_LEN(ex_list_upper_b),       ex_list_upper_b },
    { "Lower B",     ARRAY_LEN(ex_list_lower_b),       ex_list_lower_b },
    { "Full Body A", ARRAY_LEN(ex_list_full_body_a),   ex_list_full_body_a },
    { "Full Body B", ARRAY_LEN(ex_list_full_body_b),   ex_list_full_body_b },
    { "Chest",       ARRAY_LEN(ex_list_bro_chest),     ex_list_bro_chest },
    { "Back",        ARRAY_LEN(ex_list_bro_back),      ex_list_bro_back },
    { "Shoulders",   ARRAY_LEN(ex_list_bro_shoulders), ex_list_bro_shoulders },
    { "Arms",        ARRAY_LEN(ex_list_bro_arms),      ex_list_bro_arms },
    { "Legs",        ARRAY_LEN(ex_list_bro_legs),      ex_list_bro_legs },
    { "Custom 01",   ARRAY_LEN(ex_list_custom_01),     ex_list_custom_01 },
    { "Custom 02",   ARRAY_LEN(ex_list_custom_02),     ex_list_custom_02 },
};

static const workout_template_t g_templates[] = {
    { "Push Pull Legs",   0,  5 },   /* Push/Pull/Legs A + Push/Pull B */
    { "Upper Lower Arms", 5,  5 },   /* Upper A, Lower A, Arms A, Upper B, Lower B */
    { "Full Body",        10, 2 },
    { "Bro Split",        12, 5 },   /* Chest, Back, Shoulders, Arms, Legs */
    { "Custom 1",         17, 2 },
};

/* ============================================================================
 * Static list tables (menus, settings, queue, history)
 * ============================================================================ */

static const list_item_t g_main_menu[] = {
    { "Workout Logs", "" },
    { "Music Queue",  "" },
    { "Settings",     "" },
};

/* Choose Split screen (log_workout_1) */
static const list_item_t g_templates_list[] = {
    { "Push Pull Legs",   "" },
    { "Upper Lower Arms", "" },
    { "Full Body",        "" },
    { "Bro Split",        "" },
    { "Custom 1",         "" },
};

static const list_item_t g_settings_main[] = {
    { "Display",     "" },
    { "Hype & Rest", "" },
    { "Pairing",     "" },
};
static const list_item_t g_settings_display[] = {
    { "Brightness",  "75%" },
    { "Home screen", "Split view" },
    { "Theme",       "Dark" },
};
static const list_item_t g_settings_hype_rest[] = {
    { "Hype time",     "3 min" },
    { "Rest time",     "90 sec" },
    { "Hype playlist", "Varjish" },
    { "Rest playlist", "Super chill" },
};

/* Music queue (music_queue) - Now Playing header comes from spotify_state_t */
static const list_item_t g_music_queue[] = {
    { "Spaceship",                      "Kanye West" },
    { "Coldest Winter",                 "Kanye West" },
    { "White Dress",                    "Kanye West" },
    { "IFHY (ft. Pharrell)",            "Tyler, The Creator, Pharrell Williams" },
    { "El Matador",                     "Seedhe Maut" },
    { "Before I Forget",                "Slipknot" },
    { "Hurricane",                      "Kanye West" },
    { "DL91 Era",                       "Seedhe Maut" },
};

/* History list: workout + "date · duration" */
static const list_item_t g_history[] = {
    { "Push A",      "Tue 1 Sep · 32 min" },
    { "Legs A",      "Fri 29 Aug · 41 min" },
    { "Pull B",      "Mon 25 Aug · 27 min" },
    { "Upper A",     "Wed 20 Aug · 38 min" },
    { "Full Body A", "Sat 16 Aug · 45 min" },
};

/* ============================================================================
 * Small helpers
 * ============================================================================ */

/* Number of working sets (rows not labeled "Warmup*") in an exercise */
static int working_set_count(const workout_exercise_t *ex)
{
    int n = 0;
    for (int i = 0; i < ex->set_count; i++) {
        if (strncmp(ex->sets[i].label, "Warmup", 6) != 0) n++;
    }
    return n;
}

/* Weight of the k-th working set (1-based); 0 if not found */
static int work_set_weight(const workout_exercise_t *ex, int k)
{
    int n = 0;
    for (int i = 0; i < ex->set_count; i++) {
        if (strncmp(ex->sets[i].label, "Warmup", 6) != 0) {
            n++;
            if (n == k) return ex->sets[i].weight_kg;
        }
    }
    return 0;
}

/* ============================================================================
 * Workout registry accessors
 * ============================================================================ */

int workout_get_count(void)
{
    return (int)ARRAY_LEN(g_workout_plans);
}

const workout_template_t *workout_get_template(int template_index)
{
    if (template_index < 0 || template_index >= (int)ARRAY_LEN(g_templates)) return NULL;
    return &g_templates[template_index];
}

int workout_template_workout_start(int template_index)
{
    const workout_template_t *t = workout_get_template(template_index);
    return t ? t->workout_start : 0;
}

const workout_plan_t *workout_get_plan(int workout_index)
{
    if (workout_index < 0 || workout_index >= (int)ARRAY_LEN(g_workout_plans)) return NULL;
    return &g_workout_plans[workout_index];
}

/* ============================================================================
 * Screen list initializers
 * ============================================================================ */

void init_main_menu(list_screen_state_t *list)
{
    list->items = g_main_menu;
    list->count = ARRAY_LEN(g_main_menu);
    list->selected_index = 0;
    list->header = "Menu";
}

void init_template_list(list_screen_state_t *list)
{
    list->items = g_templates_list;
    list->count = ARRAY_LEN(g_templates_list);
    list->selected_index = 0;
    list->header = "Choose Split:";
}

void init_exercise_list(list_screen_state_t *list, int template_index)
{
    const workout_template_t *t = workout_get_template(template_index);
    if (!t) return;

    for (int i = 0; i < t->workout_count && i < (int)ARRAY_LEN(list->items_buf); i++) {
        const workout_plan_t *p = workout_get_plan(t->workout_start + i);
        list->items_buf[i].title = p ? p->name : "";
        list->items_buf[i].subtitle = "";
    }
    list->items = list->items_buf;
    list->count = t->workout_count;
    list->selected_index = 0;
    list->header = t->name;    /* e.g. "Push Pull Legs" frame caption */
}

void init_workout_overview(list_screen_state_t *list, int workout_index)
{
    const workout_plan_t *p = workout_get_plan(workout_index);
    if (!p) return;

    for (int i = 0; i < p->exercise_count && i < (int)ARRAY_LEN(list->items_buf); i++) {
        list->items_buf[i].title = p->exercises[i]->name;
        list->items_buf[i].subtitle = p->exercises[i]->muscles;
    }
    list->items = list->items_buf;
    list->count = p->exercise_count;
    list->selected_index = 0;
    list->header = p->name;    /* e.g. "Push A" */
}

void init_workout_session(workout_session_state_t *workout, int workout_index)
{
    const workout_plan_t *p = workout_get_plan(workout_index);
    const workout_exercise_t *ex;
    if (!p) return;

    ex = p->exercises[0];

    workout->plan = p;
    workout->exercise_index = 0;
    snprintf(workout->exercise_name, sizeof(workout->exercise_name), "%s", ex->name);

    workout->total_sets = working_set_count(ex);
    if (workout->total_sets <= 0) workout->total_sets = 1;

    /* Plan's example state: mid-way through the working sets */
    workout->current_set = (workout->total_sets < 2) ? 1 : 2;
    workout->weight_kg  = work_set_weight(ex, workout->current_set);
    workout->target_reps = 8;
    workout->completed_reps = 6;
}

void init_settings_menu(list_screen_state_t *list)
{
    list->items = g_settings_main;
    list->count = ARRAY_LEN(g_settings_main);
    list->selected_index = 0;
    list->header = "Settings";
}

void init_settings_display(list_screen_state_t *list)
{
    list->items = g_settings_display;
    list->count = ARRAY_LEN(g_settings_display);
    list->selected_index = 0;
    list->header = "Display";
}

void init_settings_hype_rest(list_screen_state_t *list)
{
    list->items = g_settings_hype_rest;
    list->count = ARRAY_LEN(g_settings_hype_rest);
    list->selected_index = 0;
    list->header = "Hype & Rest";
}

void init_music_queue(list_screen_state_t *list)
{
    list->items = g_music_queue;
    list->count = ARRAY_LEN(g_music_queue);
    list->selected_index = 0;
    list->header = "Now Playing";
}

void init_history_list(list_screen_state_t *list)
{
    list->items = g_history;
    list->count = ARRAY_LEN(g_history);
    list->selected_index = 0;
    list->header = "History";
}

/* ============================================================================
 * App state
 * ============================================================================ */

void init_app_state(app_state_t *state)
{
    memset(state, 0, sizeof(*state));

    state->current_screen  = SCREEN_HOME_IDLE;
    state->previous_screen = SCREEN_HOME_IDLE;

    init_main_menu(&state->list_state);   /* ready if the encoder opens the menu at boot */

    /* Spotify: Now Playing on the home screen (Lopaka Home_screen_both_1) */
    state->spotify.song_title = "Spaceship";
    state->spotify.artist     = "Kanye West";
    state->spotify.is_playing = true;
    state->spotify.shuffle    = false;
    state->spotify.repeat     = false;
    state->spotify.progress_seconds  = 150;   /* 2:30 */
    state->spotify.duration_seconds  = 260;   /* 4:20 */

    /* Timer: idle; total = configured duration so the boot home shows a progress bar base */
    state->timer.timer_seconds = TIMER_REST_DEFAULT;
    state->timer.total_seconds = TIMER_REST_DEFAULT;
    state->timer.is_running = false;
    state->timer.is_hype     = false;

    /* In-progress session snapshot (Push A, mid working sets, per the plan's example) */
    init_workout_session(&state->workout, 0);

    /* Last completed session summary (SCREEN_WORKOUT_COMPLETE) */
    state->workout_stats.sets_completed      = 12;
    state->workout_stats.exercises_completed = 4;
    state->workout_stats.total_volume_kg     = 8430;
    state->workout_stats.duration_min        = 28;
    state->workout_stats.strength_score      = "B+";

    /* Aggregate career/session stats (SCREEN_STATS_OVERVIEW) */
    state->stats_overview.total_workouts   = 24;
    state->stats_overview.total_sets_logged = 318;
    state->stats_overview.total_volume_kg  = 186500;
    state->stats_overview.weekly_workouts  = 3;
    state->stats_overview.weekly_minutes   = 152;
    state->stats_overview.best_lift_name   = "Barbell Squat";
    state->stats_overview.best_lift_weight_kg = 140;
    state->stats_overview.best_lift_date   = "20/6/2026";

    /* Per-exercise history (SCREEN_EXERCISE_STATS, log_workout_5) */
    snprintf(state->exercise_stats.exercise_name,
             sizeof(state->exercise_stats.exercise_name), "Incline Dumbbell Press");
    snprintf(state->exercise_stats.last_session_notes,
             sizeof(state->exercise_stats.last_session_notes), "30 degree incline");
    snprintf(state->exercise_stats.last_session_date,
             sizeof(state->exercise_stats.last_session_date), "15/4/2025");
    state->exercise_stats.last_session_top_weight_kg = 25;
    state->exercise_stats.last_session_volume_kg     = 420;
    snprintf(state->exercise_stats.pr_date,
             sizeof(state->exercise_stats.pr_date), "1/7/2025");
    state->exercise_stats.pr_weight_kg = 30;
    state->exercise_stats.pr_reps       = 8;
}