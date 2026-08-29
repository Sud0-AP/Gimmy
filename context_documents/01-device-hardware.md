# Hardware — Device

## Bill of materials (confirmed)

| Part | Role | Notes |
|---|---|---|
| Seeed Studio XIAO ESP32S3 | MCU + BLE | ~11 usable native GPIO; some shared with SPI flash — budget carefully |
| Waveshare 1.83" LCD, 240×280, SPI, IPS, 65K colors | Display | Landscape orientation. Rounded-corner glass — factor into case design later |
| M274 360° rotary encoder w/ push button | Primary scroll/select input | Quadrature (A/B) — must be on native GPIO with interrupts, NOT the I2C expander |
| 5x push buttons | Music controls (3 fixed) + Hype/Rest (2, context-dependent) | On I2C expander |
| 3-way switch, 2P2T (DPDT), ON-OFF-ON | Off / On / Lock | Pole 1: series with battery, physically cuts power in Off. Pole 2: on I2C expander, senses On vs. Lock while powered |
| PCF8574T | I2C 8-bit GPIO expander | Confirmed suitable — see pin budget below |
| NOVA 604060 LiPo battery, 3.7V, 2000mAh, protected, JST-PH connector | Power | 604060 form factor (6mm thick × 40mm wide × 60mm long) chosen for a slim profile, favoring thinness over footprint since the device is clipped to the body |
| TP4056, USB-C variant, with current protection | Charging | Single exposed USB-C port on the device |

## Pin budget

Native ESP32S3 GPIO is the scarce resource. Allocation:

- **SPI bus** (display): ~4–5 pins (MOSI, SCK, CS, DC, RST — some displays allow sharing/omitting RST)
- **I2C bus** (PCF8574T): 2 pins (SDA, SCL) — shared bus, handles up to 8 inputs
- **Rotary encoder A/B**: 2 native pins, interrupt-capable — **must not** go through the I2C expander (I2C polling is too slow/unreliable to decode quadrature rotation cleanly; missed/double-counted clicks are the failure mode to avoid)
- Everything else (5 buttons + encoder push-button + 3-way switch = 7 inputs) → PCF8574T, which has exactly 8 I/O lines. Fits with 1 spare, or exactly 8 if the 3-way switch needs 2 lines.

**Why the encoder can't share the expander with the buttons**: buttons are simple debounced digital reads — I2C polling latency is fine. The encoder needs to catch every edge transition in real time to track direction/speed accurately; that requires native interrupt-driven GPIO.

## Power system

### Battery decision (locked)

**NOVA 604060, 3.7V, 2000mAh, protected LiPo, JST-PH 2.0mm connector** (standard
connector for TP4056 boards and ESP32-adjacent hardware). Chosen in the 604060 form
factor (6mm thick × 40mm wide × 60mm long) over the equivalent 103450 option (10mm
thick × 34mm wide × 50mm long) — same capacity and price — because thickness matters
more than footprint for a device that's clipped to the body: thinness stacks directly
into overall device bulk, while footprint can be routed around in the PCB/case layout.

The 2x WLY551145 250mAh cells previously owned are repurposed for breadboard bring-up
/ current-draw testing only (Phase 0b), not the final build — their combined ~500mAh
capacity falls short of the runtime target on its own.

### Runtime target

Minimum 3–4 hours continuous use; target 4–6+ hours. A bare 250mAh cell was calculated
as insufficient (~1–1.5 hrs under continuous BLE + display load) — this is why the
2000mAh battery was chosen rather than trying to hit the runtime target through
power-saving alone. Power-saving behavior (screen dim/sleep between interactions)
still matters and should be designed in from Phase 1, but is a multiplier on top of
adequate battery capacity, not a substitute for it.

### Charging

Single USB-C port exposed through the case, wired to the TP4056 (USB-C variant, with
current protection). TP4056 output feeds the battery and the 3.3V/5V regulation stage
feeding the XIAO — standard topology, no special handling needed beyond confirming the
TP4056's charge current setting resistor is appropriate for the 604060's rated charge
current before first use.

## Power switch semantics (locked)

3-way switch, 3 states:
- **Off** — hardware power cut, battery fully disconnected from the circuit
- **On** — normal operation
- **Lock** — software lock state, not hardware power cut. Screen turns off (battery
  save), button/encoder presses are ignored/not registered, **but any active
  timer (Hype/Rest countdown, rest-between-sets, etc.) continues running normally
  in the background.** This is the state the device should be in by default when
  clipped to a person mid-workout and not actively being looked at.

## Haptics / feedback

Decision: **no buzzer** (loud clicky buttons already provide tactile/audible
confirmation, and a buzzer risks being annoying in a shared gym space). Vibration motor
is optional/nice-to-have only if a small, inexpensive option is easy to source and
integrate — not a hard requirement. Do not block any phase on sourcing a vibration
motor.

## Display orientation & UI reference

Landscape, 240×280 physical panel used in landscape mount. Screen layouts have already
been prototyped in Lopaka by the user, with generated code available, covering:
Now Playing / current exercise combined view, exercise stats (last session + PR), guided
workout logging view with editable set rows, workout template picker, split picker,
main menu (Workout Logs / Music Queue / Settings) with animated selection icons, and
music queue view (shuffle/repeat + upcoming tracks). These existing screens are the
starting visual reference for firmware rendering work in Phase 1 — see
`app/04-ui-ux.md` for the full menu map and screen-by-screen breakdown.

## Enclosure

Custom 3D-printed case, designed by the user **after** hardware is fully bring-up
tested and the pin/component layout is frozen (end of Phase 0b). No hardware decisions
in this project should be made to accommodate the case — the case is designed around
the finalized circuit, not the other way around.

## Hardware freeze checklist (must all be true before case design starts)

- [ ] Display renders correctly over SPI from the XIAO
- [ ] PCF8574T confirmed reading all 7 inputs correctly over I2C
- [ ] Rotary encoder direction/step-count confirmed accurate on native interrupt pins
- [ ] 3-way switch state detection confirmed for all 3 positions
- [ ] Battery + TP4056 charge/discharge cycle tested, real current draw measured under realistic load (screen on + BLE idle, screen on + BLE active)
- [ ] Runtime estimate recalculated from measured (not datasheet) current draw and confirmed to meet the 3–4hr minimum
