/*
 * pcf8574.h - PCF8574T I2C 8-bit expander driver (buttons)
 *
 * Buttons are active-low: a pressed button reads 0 on its pin. The expander is
 * fine for buttons (simple debounced reads) — unlike the encoder, which must
 * stay on native GPIO.
 */

#ifndef PCF8574_H
#define PCF8574_H

#include "esp_err.h"
#include <stdint.h>

/*
 * Initialize the I2C master bus and verify the PCF8574T responds at its address.
 * Returns ESP_OK on success, or an I2C error if the expander is not found.
 */
esp_err_t pcf8574_init(void);

/*
 * Read the raw 8-bit port state (P0..P7 in bits 0..7).
 * Bits are active-low: 0 = pressed, 1 = released.
 * On I2C failure, returns 0xFF (fail-safe: all released) and logs a warning.
 */
uint8_t pcf8574_read(void);

#endif // PCF8574_H
