/*
 * input_events.h - Input event types shared between input_task and app_logic_task
 *
 * The input_task owns the hardware (encoder ISR delta + PCF8574T button polls)
 * and converts raw reads into these logical events, queued to app_logic_task
 * via input_event_queue. The state machine never touches hardware directly.
 *
 * Queue transport: input_event_t -> input_event_queue (see app_config.h,
 * QUEUE_SIZE_INPUT_EVENTS) -> app_logic_task::process_input_event().
 */

#ifndef INPUT_EVENTS_H
#define INPUT_EVENTS_H

/*
 * Logical input events. Encoder rotation is coalesced into one event per detent
 * (encoder_get_delta returns +/-steps); buttons are edge-triggered on press
 * (debounced in input_task, 50ms per app_config.h).
 *
 * NOTE: On menu/list screens Hype and Rest lose their media meaning and become
 * fixed physical buttons - Hype = Back, Rest = Info (per 07-ui-interaction-spec).
 * The app_logic_task decides what each event means per current screen, so the
 * raw event names are hardware-sourced and unchanged.
 */
typedef enum {
    INPUT_ENCODER_CW,        // Clockwise rotation (1 detent)
    INPUT_ENCODER_CCW,       // Counter-clockwise rotation (1 detent)
    INPUT_ENCODER_PUSH,      // Encoder button press (select/enter)
    INPUT_BTN_PREV,          // PCF8574T P2 - Previous track
    INPUT_BTN_PLAY,          // PCF8574T P4 - Play/Pause
    INPUT_BTN_NEXT,          // PCF8574T P0 - Next track
    INPUT_BTN_HYPE,          // PCF8574T P3 - Hype timer (or Back in menus)
    INPUT_BTN_REST           // PCF8574T P1 - Rest timer (or Info in menus)
} input_event_t;

#endif // INPUT_EVENTS_H