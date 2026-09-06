/*****************************************************************************
* | File      	:   DEV_Config.h
* | Author      :   Waveshare team (ported to ESP-IDF)
* | Function    :   Hardware underlying interface for ESP32-S3
* | Info        :   ESP-IDF native SPI and GPIO driver wrapper
*----------------
* | This version:   V1.0 ESP-IDF
* | Date        :   2026-09-06
******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "app_config.h"

// Data type definitions
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

// Pin definitions (from app_config.h)
#define DEV_RST_PIN     LCD_PIN_RST
#define DEV_DC_PIN      LCD_PIN_DC
#define DEV_CS_PIN      LCD_PIN_CS
#define DEV_SPI_MOSI_PIN LCD_PIN_MOSI
#define DEV_SPI_SCK_PIN  LCD_PIN_SCK

// SPI configuration
#define DEV_SPI_HOST    LCD_SPI_HOST
#define DEV_SPI_FREQ    LCD_SPI_FREQ

// Function declarations
UBYTE DEV_Module_Init(void);
void DEV_Module_Exit(void);

void DEV_Digital_Write(UWORD Pin, UBYTE Value);
void DEV_SPI_WriteByte(UBYTE Value);
void DEV_SPI_Write_nByte(UBYTE *pData, UDOUBLE Len);
void DEV_Set_PWM(UBYTE Value);

#define DEV_SPI_WRITE(x) DEV_SPI_WriteByte(x)
#define DEV_SPI_WRite(x) DEV_SPI_WriteByte(x)

void DEV_Delay_ms(UDOUBLE xms);
void DEV_Delay_us(UDOUBLE xus);

#endif
