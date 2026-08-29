# App/Device — UI/UX

## Design system (from existing Lopaka mockups)

- **Palette**: near-black background, magenta/purple (`#c060e0`-ish) as the primary
  accent for borders, headers, and highlighted/selected states, white for primary
  readable text (exercise names, values), dimmer purple for secondary/inactive text
  and labels.
- **Typeface**: monospace/pixel-style bitmap font throughout — fits the small IPS
  panel and a hobbyist/retro-macropad aesthetic that matches the physical device.
- **Layout convention**: landscape 240×280, consistent header bar across nearly every
  non-home screen:
  - Left: contextual icons — back arrow (↩) and/or help (?) where applicable
  - Center: current time + date
  - Right: Bluetooth connection status icon, battery level icon
- **Selection state convention**: the currently-selected list item/button gets a solid
  rounded-rectangle outline in the bright accent color (vs. a thinner outline for
  unselected items) — this is the visual language for "this is what the encoder will
  activate if pressed."
- **Icon boxes**: square placeholder boxes next to menu items and exercise names are
  icon slots. Per the locked decision in the firmware doc, these are **animated when
  selected** (sprite-frame loop, e.g., spinning dumbbell) and **static when not
  selected** — not synced/arbitrary images, a fixed local icon set shipped with
  firmware.

## Screen inventory (from existing mockups, as the working reference set)

1. **Main menu** — vertical list: Workout Logs / Music Queue / Settings, with a
   scrollbar affordance on the right edge (matches encoder-scroll interaction), icon
   box beside each item.
2. **Choose Split** — list of splits (Upper Lower Arms, Push Pull Legs, Full Body,
   Custom 1, Bro Split), selected item shown in a highlighted pill/rounded box with a
   play icon, icon box on the left reserved for a split-type graphic.
3. **Choose Workout** — same list-select pattern, scoped within a chosen split (Push A,
   Pull A, Legs A, Push B, Pull B), with the split name as a caption under the icon box.
4. **Workout overview ("Push A")** — workout name + target muscles, Start and Change
   Workout buttons top-right, scrollable exercise list below (each row: exercise name +
   its specific target muscles), scrollbar on the right.
5. **Guided logging ("Push A" active session)** — workout name top-left, Stop and Home
   screen controls top-right, current exercise name + number, Stats button, and the set
   table (Set number / Weight / Repetitions columns) with the active row highlighted and
   showing up/down adjust affordances plus a checkmark to confirm, Previous/Next
   controls along the bottom to move between exercises.
6. **Exercise stats** — exercise name, freeform notes field (e.g., "30 degree
   incline"), Last Session table (sets/weight/reps + date), Personal Best table
   (sets/weight/reps + date), Back button.
7. **Now Playing / Current exercise (combined home screen variant)** — left: large
   icon box (likely album art placeholder or a music-state icon — treated as a fixed
   icon, not synced art, per the locked no-arbitrary-image decision), right: current
   exercise name, target muscle + rep scheme, and the full set table (same pattern as
   guided logging), bottom-left: track progress bar with elapsed/total time, track
   title and artist with icons, bottom-right: Hype button and the active Hype/Rest
   countdown timer.
8. **Music Queue** — shuffle/repeat/queue list screen (referenced from earlier
   discussion; matches the same list-scroll visual pattern as other menu screens) —
   shows shuffle toggle, repeat toggle, and next 5–10 upcoming tracks.

This set is the **starting visual and interaction reference** for firmware
implementation in Phase 1 — screens should be built to match this established look
rather than redesigned from scratch, with new screens (e.g., Settings, individual
Hype/Rest configuration, pairing/connection flow) designed to extend this same visual
language when needed.

## Navigation model (cross-reference: firmware doc)

- Rotary encoder rotation = scroll through any list.
- Rotary encoder push = select/confirm.
- 3 fixed buttons = global music controls, reachable from any screen.
- Hype/Rest buttons = trigger Hype/Rest on Now Playing/workout screens, act as
  Back/Home-or-Save on menu/settings/list screens (screen-based context switch, not
  press-duration-based — see firmware doc for the full rationale).

## Home screen behavior (cross-reference: firmware doc)

User-configurable and connection-state-aware: Combined (workout + music) / Music only /
Workout only. Falls back away from music-dependent variants automatically when no
phone is connected. Configured in the app; ideally mirrored in on-device settings.

## Settings screen (not yet mocked — scope for Phase 1/4 design work)

Needs to expose (at minimum), matching earlier locked decisions:
- Hype/Rest playlist assignment and duration (mirroring app-side config, for
  standalone adjustment)
- Hype/Rest second-press behavior selection (reset timer / reset+reshuffle / cancel)
- Home screen preference (combined / music / workout)
- Device pairing/connection management (forget device, re-pair)
- Battery/device info (firmware version, battery %)

This screen has not been mocked in Lopaka yet — flagged as a concrete design task
before or during Phase 4/5, following the established visual language above.

## Open questions to resolve during implementation

- Exact interaction for adjusting two independent numeric fields (weight, reps) with
  one rotary encoder during guided logging — see `03-workout-logging.md` open
  questions; this is as much a UI decision as a firmware one and should be finalized
  together.
- Final icon set list (which exercises/menu items need a bespoke icon vs. a generic
  fallback) — practical content task, not an architecture question, but worth tracking
  so it doesn't block Phase 1 menu rendering.
- Settings screen layout, once its full field list is finalized against the firmware
  and music-control docs' configurable-items lists.
