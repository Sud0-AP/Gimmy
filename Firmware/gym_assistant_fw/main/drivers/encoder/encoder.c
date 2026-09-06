/*
 * encoder.c - M274 rotary encoder driver (native GPIO + ISR)
 *
 * Ported from the Arduino bring-up sketch (Display_Encoder_Test). Uses a single
 * edge-change interrupt on the CLK line and reads DT inside the ISR to decode
 * direction — the same scheme validated on the breadboard.
 */

#include "encoder.h"
#include "app_config.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "encoder";

// ISR accumulator: raw quadrature edge counts (4 per detent on the M274).
// Read/cleared from the app task; written from the ISR. Both are plain 32-bit
// accesses, which are atomic on the ESP32-S3's single-word stores/loads.
static volatile int32_t s_raw_count = 0;
static volatile int s_last_clk = 1;

// Carries fractional detents between reads so we never lose sub-detent motion.
static int32_t s_raw_remainder = 0;

static bool s_initialized = false;

static void IRAM_ATTR encoder_isr_handler(void *arg)
{
    (void)arg;
    int clk = gpio_get_level(ENCODER_PIN_CLK);
    if (clk != s_last_clk) {
        // On each CLK edge, DT != CLK means one direction, DT == CLK the other.
        if (gpio_get_level(ENCODER_PIN_DT) != clk) {
            s_raw_count++;
        } else {
            s_raw_count--;
        }
    }
    s_last_clk = clk;
}

esp_err_t encoder_init(void)
{
    if (s_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    gpio_config_t io_conf = {
        .pin_bit_mask = ((1ULL << ENCODER_PIN_CLK) | (1ULL << ENCODER_PIN_DT)),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,  // CHANGE-equivalent on CLK
    };
    esp_err_t ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gpio_config failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // DT is read-only inside the ISR; disable its interrupt so only CLK fires.
    gpio_set_intr_type(ENCODER_PIN_DT, GPIO_INTR_DISABLE);

    s_last_clk = gpio_get_level(ENCODER_PIN_CLK);

    // Install the shared GPIO ISR service (idempotent across drivers).
    ret = gpio_install_isr_service(0);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "gpio_install_isr_service failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = gpio_isr_handler_add(ENCODER_PIN_CLK, encoder_isr_handler, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gpio_isr_handler_add failed: %s", esp_err_to_name(ret));
        return ret;
    }

    s_initialized = true;
    ESP_LOGI(TAG, "encoder initialized (CLK=GPIO%d, DT=GPIO%d)",
             ENCODER_PIN_CLK, ENCODER_PIN_DT);
    return ESP_OK;
}

int32_t encoder_get_delta(void)
{
    // On 32-bit ESP32-S3, reading a volatile int32_t is atomic.
    // No critical section needed — avoids starving the IDLE task watchdog.
    int32_t raw = s_raw_count;
    s_raw_count = 0;

    // Convert raw quadrature edges to detents, preserving the remainder.
    raw += s_raw_remainder;
    int32_t detents = raw / ENCODER_STEPS_PER_DETENT;
    s_raw_remainder = raw - (detents * ENCODER_STEPS_PER_DETENT);

    return detents;
}
