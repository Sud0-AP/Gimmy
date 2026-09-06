/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team (ported to ESP-IDF)
* | Function    :   Hardware underlying interface for ESP32-S3
* | Info        :   ESP-IDF native SPI and GPIO driver wrapper
*----------------
* | This version:   V1.0 ESP-IDF
* | Date        :   2026-09-06
******************************************************************************/
#include "DEV_Config.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include <string.h>

static const char *TAG = "DEV_Config";

// SPI device handle
spi_device_handle_t spi_handle = NULL;

/******************************************************************************
function:	Module Initialize, the BCM2835 library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE DEV_Module_Init(void)
{
    ESP_LOGI(TAG, "DEV_Module_Init");

    // Configure GPIO pins for LCD control
    gpio_config_t io_conf = {
        .pin_bit_mask = ((1ULL << DEV_RST_PIN) | (1ULL << DEV_DC_PIN) | (1ULL << DEV_CS_PIN)),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    // Set initial pin states
    gpio_set_level(DEV_CS_PIN, 1);
    gpio_set_level(DEV_DC_PIN, 0);
    gpio_set_level(DEV_RST_PIN, 1);

    // Initialize SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = DEV_SPI_MOSI_PIN,
        .miso_io_num = -1,  // Not used for this display
        .sclk_io_num = DEV_SPI_SCK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 64 * 1024,  // 64KB max transfer
    };

    esp_err_t ret = spi_bus_initialize(DEV_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return 1;
    }

    // Attach the LCD to the SPI bus
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = DEV_SPI_FREQ,
        .mode = 0,  // SPI mode 0
        .spics_io_num = DEV_CS_PIN,
        .queue_size = 7,
        .pre_cb = NULL,
    };

    ret = spi_bus_add_device(DEV_SPI_HOST, &devcfg, &spi_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %s", esp_err_to_name(ret));
        return 1;
    }

    ESP_LOGI(TAG, "DEV_Module_Init OK");
    return 0;
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_Module_Exit(void)
{
    ESP_LOGI(TAG, "DEV_Module_Exit");
    if (spi_handle != NULL) {
        spi_bus_remove_device(spi_handle);
        spi_handle = NULL;
    }
    spi_bus_free(DEV_SPI_HOST);
}

/******************************************************************************
function:	Hardware reset
parameter:
Info:
******************************************************************************/
void DEV_Digital_Write(UWORD Pin, UBYTE Value)
{
    gpio_set_level(Pin, Value);
}

/******************************************************************************
function:	SPI transfer
parameter:
Info:
******************************************************************************/
void DEV_SPI_WriteByte(UBYTE Value)
{
    spi_transaction_t t = {
        .length = 8,  // 8 bits
        .tx_buffer = &Value,
        .rx_buffer = NULL,
    };
    spi_device_polling_transmit(spi_handle, &t);
}

void DEV_SPI_Write_nByte(UBYTE *pData, UDOUBLE Len)
{
    if (Len == 0) {
        return;
    }

    spi_transaction_t t = {
        .length = Len * 8,  // Length in bits
        .tx_buffer = pData,
        .rx_buffer = NULL,
    };
    spi_device_polling_transmit(spi_handle, &t);
}

/******************************************************************************
function:	Millisecond delay
parameter:
Info:
******************************************************************************/
void DEV_Delay_ms(UDOUBLE xms)
{
    vTaskDelay(pdMS_TO_TICKS(xms));
}

/******************************************************************************
function:	Microsecond delay
parameter:
Info:
******************************************************************************/
void DEV_Delay_us(UDOUBLE xus)
{
    esp_rom_delay_us(xus);
}

/******************************************************************************
function:	PWM brightness control (stub for Phase 1 — BL tied to 3V3)
parameter:
Info:	Phase 5 will wire BL to a PWM GPIO for brightness control
******************************************************************************/
void DEV_Set_PWM(UBYTE Value)
{
    // Phase 1: Backlight is tied to 3V3 (always full brightness)
    // Phase 5: Add PWM GPIO configuration here
    (void)Value;  // Unused in Phase 1
}
