/*
 * main.c - Gym Assistant Firmware Entry Point
 * Phase 1: Hardware initialization verification
 *
 * Initializes all three peripherals (LCD, encoder, PCF8574T) and confirms they
 * come up. Draws a boot screen and polls inputs so the breadboard can be
 * exercised before the FreeRTOS task architecture lands in Phase 3.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "app_config.h"
#include "DEV_Config.h"
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "fonts.h"
#include "encoder.h"
#include "pcf8574.h"

static const char *TAG = "MAIN";

static void draw_boot_screen(void)
{
    Paint_Clear(COLOR_BG);
    vTaskDelay(1);  // Let IDLE task run after the 67K-pixel clear

    Paint_DrawString_EN(15, 10, "Gym Assistant", &Font24, COLOR_BG, COLOR_PRIMARY);
    vTaskDelay(1);
    Paint_DrawString_EN(15, 45, "Phase 1: Bring-up", &Font12, COLOR_BG, COLOR_TEXT_PRIMARY);
    vTaskDelay(1);

    Paint_DrawLine(15, 65, 265, 65, COLOR_TEXT_SECONDARY, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    vTaskDelay(1);

    Paint_DrawString_EN(15, 80, "Display:  OK", &Font12, COLOR_BG, COLOR_TEXT_PRIMARY);
    vTaskDelay(1);
    Paint_DrawString_EN(15, 100, "Encoder:  ready", &Font12, COLOR_BG, COLOR_TEXT_PRIMARY);
    vTaskDelay(1);
    Paint_DrawString_EN(15, 120, "Buttons:  ready", &Font12, COLOR_BG, COLOR_TEXT_PRIMARY);
    vTaskDelay(1);

    Paint_DrawString_EN(15, 150, "Rotate & press to test", &Font12, COLOR_BG, COLOR_TEXT_SECONDARY);
}

void app_main(void)
{
    ESP_LOGI(TAG, "==============================================");
    ESP_LOGI(TAG, "Gym Assistant Firmware - Phase 1");
    ESP_LOGI(TAG, "ESP-IDF Version: %s", esp_get_idf_version());
    ESP_LOGI(TAG, "==============================================");

    // --- Display init ---
    if (DEV_Module_Init() != 0) {
        ESP_LOGE(TAG, "DEV_Module_Init failed");
        return;
    }
    LCD_Init();
    LCD_SetBacklight(255);

    // STM32 GUI_Paint uses callback-based rendering (4-arg Paint_NewImage)
    Paint_NewImage(LCD_1IN83_WIDTH, LCD_1IN83_HEIGHT, ROTATE_90, WHITE);
    Paint_SetClearFuntion(LCD_1IN83_Clear);
    Paint_SetDisplayFuntion(LCD_1IN83_DrawPoint);
    ESP_LOGI(TAG, "Display initialized");

    // --- Encoder init ---
    if (encoder_init() != ESP_OK) {
        ESP_LOGE(TAG, "encoder_init failed");
    } else {
        ESP_LOGI(TAG, "Encoder initialized");
    }

    // --- Buttons (PCF8574T) init ---
    bool buttons_ok = (pcf8574_init() == ESP_OK);
    if (buttons_ok) {
        ESP_LOGI(TAG, "I2C initialized");
    } else {
        ESP_LOGE(TAG, "pcf8574_init failed (check wiring)");
    }

    draw_boot_screen();

    ESP_LOGI(TAG, "Phase 1 init complete — polling inputs");

    // Simple polling loop to exercise inputs on the breadboard. This is a
    // Phase 1 smoke test; Phase 3 replaces it with the input_task + queues.
    int32_t encoder_total = 0;
    uint8_t last_buttons = buttons_ok ? pcf8574_read() : 0xFF;

    while (1) {
        int32_t delta = encoder_get_delta();
        if (delta != 0) {
            encoder_total += delta;
            ESP_LOGI(TAG, "Encoder %s (total=%ld)",
                     delta > 0 ? "CW" : "CCW", (long)encoder_total);
        }

        if (buttons_ok) {
            uint8_t buttons = pcf8574_read();
            if (buttons != last_buttons) {
                for (int i = 0; i < 8; i++) {
                    bool was_pressed = !(last_buttons & (1 << i));
                    bool is_pressed = !(buttons & (1 << i));
                    if (is_pressed && !was_pressed) {
                        ESP_LOGI(TAG, "Button P%d pressed", i);
                    } else if (!is_pressed && was_pressed) {
                        ESP_LOGI(TAG, "Button P%d released", i);
                    }
                }
                last_buttons = buttons;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
