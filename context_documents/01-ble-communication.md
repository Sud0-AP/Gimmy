# App — BLE Communication (Device ⇄ Phone)

## Role split (locked)

The device **never** talks to Spotify or any external API directly. It only speaks BLE
to the companion app. The app is responsible for all OAuth, all Spotify Web API /
Spotify Connect calls, and the full workout database. This keeps the device's BLE stack
simple and keeps all credential/token handling off the embedded side entirely.

## Pairing / connection model (locked)

- **Single bonded phone at a time.** The device remembers one paired phone; no
  multi-device switching UI needed for v1. (If a different phone needs to pair later —
  e.g., testing an open-source build — that's a re-pair/forget-device action, not a
  saved list of multiple trusted devices.)
- Standard BLE bonding flow; no additional pairing-confirmation-code UX was deemed
  necessary beyond normal OS-level BLE pairing (no strong opinion held here — implement
  whatever is simplest and standard, revisit only if a real security concern surfaces).

## Message protocol

No hard requirement on binary vs JSON-style — pick whichever is easiest to implement,
debug, and **extend later**, since extensibility for a future open-source audience
matters more than raw efficiency at this data volume (these are small, infrequent
control messages, not high-throughput streams). A simple versioned, tagged message
schema (e.g., a compact JSON-like or TLV structure with a `type` field and a schema
version) is a reasonable default — document the chosen format concretely once
implementation starts, including a version field from message 1 so the protocol can
evolve without breaking old firmware/app pairings.

## Message categories

**Device → App**
- Playback commands: play/pause, next/previous, volume up/down, playlist select,
  queue navigation (shuffle toggle, repeat toggle, skip within queue)
- Hype/Rest trigger events (which button, at what timestamp) and cancel/reset events
- Workout log events: set completed (exercise id, set number, weight, reps), session
  started/ended
- Sync requests: "send me updated templates," "here is my pending log queue, please
  ack"
- Device status: battery level, firmware version (useful for future OTA gating and for
  app-side "device needs charging" prompts)

**App → Device**
- Now-playing metadata: track title, artist, playback position/duration, and **album
  art** for the current track (confirmed required by the Home screens — see
  `02-music-control.md` and `07-ui-interaction-spec.md`). The transfer format
  (raw bitmap vs. compressed JPEG decoded on-device) is a Phase 2 schema decision — see
  open questions below.
- Playlist name list (for the pre-configured playlist picker) and queue contents
  (upcoming tracks, shuffle/repeat state) for the Music Queue screen
- Workout templates (exercises, target sets/reps/weight, last-session and PR data) —
  the 2–3 template cache described in the firmware doc
- Settings sync: Hype/Rest durations, second-press behavior, home screen preference,
  etc. (bi-directional in practice — device can also change these locally and should
  push changes back up when connected)
- Sync acknowledgements for received workout logs
- Connection/pairing housekeeping

## Reliability model (locked — this is a designed-for scenario, not an edge case)

The device is the **source of truth for workout state**. BLE connectivity is treated as
inherently unreliable, especially on iOS (see background BLE note below), so:
- Workout logging, timers, and guided workout flow must work fully with the connection
  dropped, and reconcile automatically once it's back (see firmware doc's offline-first
  section).
- Music commands sent while disconnected should fail gracefully on-device (e.g., show
  a "not connected" indicator rather than appearing to succeed) — there's no meaningful
  way to queue-and-replay a "next track" command after the fact, so these are
  fire-and-forget only when connected.
- Reconnection should be automatic and silent from the user's perspective whenever
  possible — no manual "reconnect" button should be required for normal drop/recover
  cycles.

## Background operation constraints (platform-specific — locked understanding)

- **Android**: Reliable background BLE requires a **foreground service**, which
  requires a persistent (non-dismissible) notification while active. Design decision:
  only show this notification while a session is actively connected (not permanently
  running at all times), and make its content useful (current track / workout status)
  rather than a blank placeholder.
- **iOS**: Uses CoreBluetooth's `bluetooth-central` background mode, which does **not**
  require a persistent notification and can wake the app on BLE events even when
  backgrounded/locked. Tradeoffs: background scanning/reconnection timing is throttled
  by the OS and less deterministic than Android's foreground-service model; if the OS
  or user fully terminates the app (not just backgrounds it), reconnection on next
  launch needs to be handled via CoreBluetooth **state restoration**, which must be
  implemented explicitly (not automatic).
- Net implication: iOS users will see more frequent brief disconnects than Android
  users. This is expected, not a bug to chase — the offline-first device design absorbs
  it.

## Future phase (noted, not in scope yet)

OTA update capability — app pushing firmware/theme/settings updates to the device over
BLE — is a planned future phase, not part of the initial protocol design. The message
schema should include a version field partly so this can be added later without
breaking compatibility, but no OTA transport work happens until its own phase.

## Open questions to resolve during implementation (not blocking doc creation)

- Exact message serialization format (concrete schema) — decide when Phase 2 begins.
- **Resolved: album art IS sent over BLE.** The Home screens require real synced album
  art for the current track (see `02-music-control.md`), so the earlier "fixed local
  icons only" lean no longer holds. What remains open is the **format** — app sends a
  ready-to-draw raw bitmap (no on-device decoder, larger payload) vs. compressed JPEG
  (smaller payload, needs an on-device decoder). Decide this when the Phase 2 BLE schema
  is designed.
