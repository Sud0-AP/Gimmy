# App — Workout Logging & Guided Workouts

## Division of responsibility (locked)

- **Template/library creation happens exclusively in the app.** The device has no
  text entry capability (no keyboard), so exercises, splits, workout templates, target
  sets/reps/weight schemes are all authored on the phone.
- **The device executes and logs against templates.** Its job during a session is:
  show the plan, let the user scroll/select/confirm via the encoder, and record actual
  performance (weight/reps per set) as numeric values adjustable via the encoder.
- **Full history and long-term analytics live in the app's database.** The device only
  ever holds a small rolling cache (see firmware doc: 2–3 recent templates + their
  last-session/PR data) so it can operate standalone.

## Workout selection flow (reference: existing Lopaka screens)

Matches the "Choose Split" → "Choose Workout" → workout detail screens already
designed:
1. **Choose Split** (e.g., "Push Pull Legs," "Upper Lower Arms," "Full Body," custom
   splits) — encoder-scrollable list.
2. **Choose Workout** within that split (e.g., "Push A," "Pull A," "Legs A") —
   same list-select pattern.
3. **Workout detail/start screen** — shows workout name, target muscles, and the
   exercise list with a Start button and a Change Workout option, matching the "Push A"
   overview screen.

## Guided workout flow (locked)

Once started, the device steps through the workout's exercise list. For each exercise,
the screen shows (matching the existing "log workout" Lopaka screen):
- Exercise name and target muscle groups
- The full set breakdown: warmup sets and work sets, each with a **target** weight and
  rep range (e.g., "Work Set 1: 25kg × 6–8")
- The currently-selected set row is editable via the encoder: scroll to adjust weight,
  scroll (or a secondary control) to adjust reps, confirm/check off with the encoder
  push-button — matching the up/down arrow + checkmark pattern already in the mockup
- Previous / Next navigation between exercises within the workout
- A **Stats** button/screen showing that exercise's last session performance and
  personal best (matches the "Exercise stats" Lopaka screen: last session sets/weight/
  reps with date, and personal best sets/weight/reps with date) — this is pulled from
  the on-device cache when available, or requested from the app if not cached
- A **Stop** control to end the workout session early, and a **Home screen** shortcut

This confirms and extends the earlier scoping: guided workout = numeric logging against
pre-set targets with historical context shown, not a simple pass/fail checklist.

## Rest timing (locked — explicitly separate from Hype/Rest music feature)

There is **no automatic rest-between-sets timer tied to logging a set**. This was a
deliberate decision: real gym sessions have unpredictable social/equipment-availability
gaps between sets that don't fit a rigid auto-timer model. If the user wants
music-backed rest support, they manually press the Rest button (see
`02-music-control.md`) whenever they choose to. Workout progress tracking and music
timing are independent systems that happen to be usable together, not one triggering
the other.

## Offline behavior (locked — see firmware doc for storage detail)

The device must be able to run an entire guided workout session, log all sets, and
show stats **with zero phone connection**, using the on-device cache of 2–3 recent
templates. When a connection is available, logged data syncs up to the app's full
database, and updated templates/settings sync down. This is a first-class requirement,
not a fallback path.

## Data needed on-device per cached template (for offline operation)

- Template structure: split name, workout name, ordered exercise list
- Per exercise: name, target muscle groups, warmup set count/target, work set
  count/target (weight range, rep range)
- Per exercise: last-session actuals (sets/weight/reps, date) and personal-best
  actuals (sets/weight/reps, date) — enough to render the Stats screen offline
- Any playlist association, if templates are ever linked to specific music (not
  currently specified as a requirement — flag as an open question rather than assumed)

## Open questions to resolve during implementation

- Exact mechanism for adjusting reps vs. weight independently with a single rotary
  encoder (e.g., encoder scroll = weight, a secondary button = switch field to reps, vs.
  short-press to toggle which field is "active" before scrolling) — needs a concrete UX
  decision during Phase 4, informed by hands-on testing of what feels fast mid-set.
- Whether workout templates can ever be associated with a specific playlist (auto-
  suggest music per workout type) — not currently a stated requirement, worth
  confirming is out of scope for v1 before Phase 4 begins.
- Exact sync/conflict rules if the same template is edited in the app while a cached
  version is mid-use on the device (low priority — unlikely in single-user personal use,
  but worth a documented rule before open-sourcing).
