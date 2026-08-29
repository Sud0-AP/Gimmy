# App — Music Control

## Scope & integration

Spotify only for v1 (locked) — it's currently the only mainstream music API offering
the level of granular playback/queue control this project needs. The app owns all
Spotify OAuth and API calls; the device only ever sends semantic commands over BLE
(see `01-ble-communication.md`). Code should avoid hard-wiring Spotify assumptions
everywhere it doesn't need to (keep the command layer generic — "next track," "play
playlist X" — rather than literally shaping data structures around Spotify's API
responses) so a future service swap is realistic, without spending real effort on that
abstraction now.

## Standard playback controls (locked)

- Play / Pause
- Next / Previous track
- Volume up / down (see Volume section below — important platform caveat)
- Playlist selection (from a **pre-configured shortlist**, not full library browsing —
  see below)
- Song/track switching within the current playlist or queue

## Playlist browsing model (locked)

The device does **not** browse the user's full Spotify library. Instead:
- The user selects/curates a shortlist of playlists **in the app**, which is what
  appears in the on-device playlist picker (matches the "Choose Split" / "Choose
  workout" Lopaka screen pattern — same list-select interaction, applied to playlists).
- The **Music Queue** menu screen (see existing Lopaka screen) shows: shuffle toggle,
  repeat toggle, and the next 5–10 upcoming tracks in the current queue, scrollable via
  the encoder.
- This keeps on-device data light (no full-library sync needed) and keeps the
  interaction fast — the whole point is minimal on-device decision-making.

## Volume control (locked understanding — platform-asymmetric)

**What it actually controls**: the device has no audio output itself. "Volume"
commands are relayed over BLE to the app, which changes the volume of whichever
playback surface is actually producing sound — this could be interpreted as either (a)
the Spotify Connect playback device's volume via the Spotify API, or (b) the phone's
system media volume, depending on what's actually driving the user's audio (phone
speaker/wired output vs. a separate Spotify Connect-capable speaker/headphone).

**Decision**: target **system volume control** (raising/lowering the phone's overall
media volume), since the user's real-world setup is Bluetooth headphones/speakers
connected to the phone, not a separate Spotify Connect endpoint.

- **Android**: apps can programmatically change system media volume without user
  interaction — implement as a clean, silent volume change.
- **iOS**: Apple does **not** allow silent system volume changes from within an app.
  The only mechanism is triggering the native system volume HUD (a visible slider),
  which cannot be suppressed. **Decision: best-effort on iOS** — implement using the
  native volume view/HUD trigger, document clearly that a brief on-screen indicator is
  an unavoidable platform limitation, not a bug. Do not attempt private-API workarounds.

## Hype / Rest — flagship feature (fully locked)

Two dedicated physical buttons: **Hype** and **Rest**. Each independently configurable
in the app (durations editable in both app and on-device settings where feasible):

1. **Trigger**: pressing the button (only meaningful on Now Playing / active workout
   screens — see firmware doc's context-button rule) does the following, atomically:
   - Snapshots current playback state: track, position, queue/context reference,
     shuffle state, repeat state — everything needed to restore playback exactly as it
     was.
   - Starts the user-configured **target playlist** for that button.
   - Starts a countdown timer for the user-configured **duration** for that button.
2. **On-screen**: shows the countdown (see the Lopaka "Current exercise" screen's
   `⏳ 2:30` element as the reference UI pattern) alongside whatever else is on screen
   (workout view, Now Playing view).
3. **On expiry**: playback state is restored exactly from the snapshot — same track,
   same position (or as close as the Spotify API allows), same queue/shuffle/repeat
   state.
4. **Second-press behavior during an active Hype/Rest is user-configurable**, one of:
   - Reset the timer (restart the countdown, keep playing the Hype/Rest playlist)
   - Reset the timer **and** reshuffle/restart the Hype/Rest playlist
   - Cancel Hype/Rest entirely and immediately restore the pre-snapshot playback state
   - (Exact option set/labels to be finalized in the app settings UI — the underlying
     requirement is that this behavior is a setting, not hardcoded)
5. **Stacking behavior**: pressing the *other* button while one is already active — 
   exact behavior TBD during implementation, but the reasonable default (to confirm
   when building) is: pressing Rest while Hype is active (or vice versa) treats it like
   a fresh trigger of the newly-pressed mode — snapshot is taken from whatever is
   currently playing (which may already be a Hype/Rest playlist), old timer is
   discarded, new mode's playlist+timer starts. This avoids ambiguous nested-snapshot
   logic. Revisit if this proves confusing in practice.

## Relationship to workout rest timers (locked — explicitly separate)

The Hype/Rest music feature is **entirely independent** from any rest-between-sets
timing in the guided workout flow. Real gym sessions involve unpredictable social time
between sets (chatting, waiting for equipment, etc.), so a rigid auto-linked
rest-timer-triggers-music model was explicitly rejected. Users can manually hit the
Rest button whenever they want music-based rest support — it is not automatically tied
to logging a set. See `03-workout-logging.md` for how set logging and rest timing are
handled on their own terms.

## Now Playing screen data (reference: existing Lopaka screens)

Displays (per the user's existing designs): track title, artist, elapsed/total time
with progress bar, and — when relevant — the current exercise context (target
muscle/rep scheme) alongside it on the combined home screen variant. No arbitrary
album art rendering planned (keeps BLE payload and on-device rendering simple) —
placeholder icon boxes shown in the mockups are treated as fixed local icon assets, not
synced images, pending final confirmation in the UI doc.
