/*
 * pcf8574.c - PCF8574T I2C 8-bit expander driver (buttons)
 *
 * Ported from the Arduino bring-up sketch (Display_Encoder_Test), which used
 * Wire.requestFrom()/Wire.read(). Uses the ESP-IDF v5.2+ i2c_master driver.
 */

#include "pcf8574.h"
#include "app_config.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

static const char *TAG = "pcf8574";

static i2c_master_bus_handle_t s_bus = NULL;
static i2c_master_dev_handle_t s_dev = NULL;

esp_err_t pcf8574_init(void)
{
    i2c_master_bus_config_t bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_MASTER_NUM,
        .scl_io_num = I2C_PIN_SCL,
        .sda_io_num = I2C_PIN_SDA,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    esp_err_t ret = i2c_new_master_bus(&bus_cfg, &s_bus);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c_new_master_bus failed: %s", esp_err_to_name(ret));
        return ret;
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = PCF8574_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };
    ret = i2c_master_bus_add_device(s_bus, &dev_cfg, &s_dev);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c_master_bus_add_device failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // Probe the expander to confirm it's wired and addressed correctly.
    ret = i2c_master_probe(s_bus, PCF8574_ADDR, 100);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PCF8574T not found at 0x%02X: %s", PCF8574_ADDR,
                 esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "PCF8574T initialized at 0x%02X (SDA=GPIO%d, SCL=GPIO%d)",
             PCF8574_ADDR, I2C_PIN_SDA, I2C_PIN_SCL);
    return ESP_OK;
}

uint8_t pcf8574_read(void)
{
    uint8_t data = 0xFF;  // fail-safe default: all released
    esp_err_t ret = i2c_master_receive(s_dev, &data, 1, 100);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "read failed: %s", esp_err_to_name(ret));
        return 0xFF;
    }
    return data;
}
