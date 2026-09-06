/*
 * app_config.h - Hardware pin definitions and configuration constants
 * for Gym Assistant Device
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "driver/gpio.h"
#include "driver/spi_master.h"

// ============================================================================
// XIAO ESP32S3 Pin Mapping
// ============================================================================

// LCD (Waveshare 1.83" IPS, NV3030B controller, SPI)
#define LCD_PIN_CS      GPIO_NUM_1    // D0 - MUST be D0 (D9 conflicts with MISO)
#define LCD_PIN_DC      GPIO_NUM_2    // D1 - Data/Command
#define LCD_PIN_RST     GPIO_NUM_4    // D3 - Reset
#define LCD_PIN_MOSI    GPIO_NUM_9    // D10 - Hardware SPI MOSI
#define LCD_PIN_SCK     GPIO_NUM_7    // D8 - Hardware SPI SCK
// LCD_BL tied to 3V3 (always on for Phase 1, move to PWM GPIO for dimming in Phase 5)

// Rotary Encoder (M274 360° encoder, native GPIO with interrupts)
#define ENCODER_PIN_CLK GPIO_NUM_43   // D6 - CLK channel (interrupt pin)
#define ENCODER_PIN_DT  GPIO_NUM_44   // D7 - Direction decode

// I2C (PCF8574T expander for buttons)
#define I2C_PIN_SDA     GPIO_NUM_5    // D4
#define I2C_PIN_SCL     GPIO_NUM_6    // D5
#define I2C_MASTER_NUM  I2C_NUM_0
#define I2C_FREQ_HZ     100000        // 100kHz

// PCF8574T I2C address
#define PCF8574_ADDR    0x20

// PCF8574T Button Mapping (P0-P7)
#define PCF8574_BTN_NEXT      (1 << 0)  // P0 - Next track
#define PCF8574_BTN_REST      (1 << 1)  // P1 - Rest button
#define PCF8574_BTN_PREV      (1 << 2)  // P2 - Previous track
#define PCF8574_BTN_HYPE      (1 << 3)  // P3 - Hype button
#define PCF8574_BTN_PLAY      (1 << 4)  // P4 - Play/Pause
// P5-P7: encoder push-button + 3-way switch (not yet assigned)

// ============================================================================
// Display Configuration
// ============================================================================

#define LCD_WIDTH       240
#define LCD_HEIGHT      280
#define LCD_ROTATION    90              // Landscape mode (280x240 logical)

// SPI Configuration for LCD
#define LCD_SPI_HOST    SPI2_HOST
#define LCD_SPI_FREQ    40000000        // 40MHz (max for NV3030B)
#define LCD_DMA_CHAN    SPI_DMA_CH_AUTO

// ============================================================================
// FreeRTOS Task Configuration
// ============================================================================

// Task priorities (higher number = higher priority)
#define TASK_PRIORITY_DISPLAY   5       // High - owns SPI, 24 FPS animations
#define TASK_PRIORITY_INPUT     5       // High - encoder ISR, I2C polling
#define TASK_PRIORITY_APP_LOGIC 3       // Medium - state machine, timers
#define TASK_PRIORITY_BLE       3       // Medium - BLE GATT (stub for Phase 1)
#define TASK_PRIORITY_STORAGE   1       // Low - NVS/LittleFS (stub for Phase 1)

// Task stack sizes (bytes)
#define TASK_STACK_DISPLAY      4096
#define TASK_STACK_INPUT        2048
#define TASK_STACK_APP_LOGIC    4096
#define TASK_STACK_BLE          4096
#define TASK_STACK_STORAGE      2048

// Queue sizes
#define QUEUE_SIZE_INPUT_EVENTS 16      // Input events from input_task
#define QUEUE_SIZE_RENDER       4       // Render commands to display_task

// ============================================================================
// UI Configuration
// ============================================================================

// Design Tokens (RGB565 color palette)
#define COLOR_BG            0x0801      // Near-black background
#define COLOR_PRIMARY       0xE33F      // Bright magenta (accent, selection)
#define COLOR_TEXT_PRIMARY  0xFFFF      // White (primary text)
#define COLOR_TEXT_SECONDARY 0x6870     // Medium gray (secondary text)
#define COLOR_BORDER_INACTIVE 0x506C    // Dark gray (inactive borders)

// Animation Configuration
#define ANIM_FPS            24          // Target frame rate for selection animations
#define ANIM_FRAME_MS       42          // 1000/24 ≈ 42ms per frame

// ============================================================================
// Input Configuration
// ============================================================================

// Encoder sensitivity
#define ENCODER_STEPS_PER_DETENT 4      // M274 encoder quadrature steps

// Button debounce
#define BUTTON_DEBOUNCE_MS  50          // Minimum time between button events

// I2C polling rate
#define I2C_POLL_RATE_HZ    20          // Poll PCF8574T at 20Hz
#define I2C_POLL_DELAY_MS   (1000 / I2C_POLL_RATE_HZ)

// ============================================================================
// App Logic Configuration
// ============================================================================

// Timer defaults (seconds)
#define TIMER_HYPE_DEFAULT  180         // 3 minutes
#define TIMER_REST_DEFAULT  90          // 90 seconds

// Workout session defaults
#define WEIGHT_INCREMENT_KG 5           // Weight adjustment step
#define WEIGHT_MIN_KG       0           // Minimum weight (can be 0 for bodyweight)

#endif // APP_CONFIG_H
