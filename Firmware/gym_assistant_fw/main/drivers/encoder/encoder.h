/*
 * encoder.h - M274 rotary encoder driver (native GPIO + ISR)
 *
 * The encoder MUST stay on native GPIO — routing A/B through the PCF8574T
 * expander drops quadrature edges due to I2C polling latency.
 */

#ifndef ENCODER_H
#define ENCODER_H

#include "esp_err.h"
#include <stdint.h>

/*
 * Install the CLK-edge ISR and configure both encoder pins as inputs.
 * Safe to call once at startup; returns ESP_ERR_INVALID_STATE if called twice.
 */
esp_err_t encoder_init(void);

/*
 * Read and clear the accumulated detent delta since the last call.
 * Positive = clockwise, negative = counter-clockwise.
 *
 * The caller (input_task) owns the polling cadence; this function is the
 * only consumer of the ISR's accumulator, so the read-and-clear is atomic
 * with respect to the ISR.
 */
int32_t encoder_get_delta(void);

#endif // ENCODER_H
