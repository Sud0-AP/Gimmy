/*****************************************************************************
* | File        :   LCD_1in9.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :   Used to shield the underlying layers of each master and enhance portability
*----------------
* | This version:   V1.0
* | Date        :   2022-11-02
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __LCD_1IN9_H
#define __LCD_1IN9_H   
    
#include "DEV_Config.h"
#include <stdint.h>

#include <stdlib.h>     //itoa()
#include <stdio.h>

#define LCD_1IN9_HEIGHT 170
#define LCD_1IN9_WIDTH 320

#define HORIZONTAL 0
#define VERTICAL   1

#define LCD_1IN9_CS_0  DEV_Digital_Write(DEV_CS_PIN, 0)     
#define LCD_1IN9_CS_1  DEV_Digital_Write(DEV_CS_PIN, 1)    
                     
#define LCD_1IN9_RST_0 DEV_Digital_Write(DEV_RST_PIN, 0)
#define LCD_1IN9_RST_1 DEV_Digital_Write(DEV_RST_PIN, 1)   
                      
#define LCD_1IN9_DC_0  DEV_Digital_Write(DEV_DC_PIN, 0)
#define LCD_1IN9_DC_1  DEV_Digital_Write(DEV_DC_PIN, 1)
                      
typedef struct{
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
}LCD_1IN9_ATTRIBUTES;
extern LCD_1IN9_ATTRIBUTES LCD_1IN9;

/********************************************************************************
function:   Macro definition variable name
********************************************************************************/
void LCD_1IN9_Init(UBYTE Scan_dir);
void LCD_1IN9_Clear(UWORD Color);
void LCD_1IN9_Display(UWORD *Image);
void LCD_1IN9_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_1IN9_DrawPoint(UWORD X, UWORD Y, UWORD Color);
void LCD_1IN9_SetBackLight(UWORD Value);
#endif
