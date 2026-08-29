/*****************************************************************************
* | File        :   LCD_1in83.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :   Used to shield the underlying layers of each master and enhance portability
*----------------
* | This version:   V1.0
* | Date        :   2024-03-05
* | Info        :   Basic version
 *
 ******************************************************************************/
#include "LCD_1in83.h"
#include "DEV_Config.h"

#include <stdlib.h> //itoa()
#include <stdio.h>

LCD_1IN83_ATTRIBUTES LCD_1IN83;

/******************************************************************************
function :  Hardware reset
parameter:
******************************************************************************/
static void LCD_1IN83_Reset(void)
{
    LCD_1IN83_RST_1;
    DEV_Delay_ms(10);
    LCD_1IN83_RST_0;
    DEV_Delay_ms(10);
    LCD_1IN83_RST_1;
    DEV_Delay_ms(120);
}

/******************************************************************************
function :  send command
parameter:
     Reg : Command register
******************************************************************************/
static void LCD_1IN83_SendCommand(UBYTE Reg)
{
    LCD_1IN83_DC_0;
    LCD_1IN83_CS_0;
    DEV_SPI_WRITE(Reg);
    //LCD_1IN83_CS_1;
}

/******************************************************************************
function :  send data
parameter:
    Data : Write data
******************************************************************************/
static void LCD_1IN83_SendData_8Bit(UBYTE Data)
{
    LCD_1IN83_DC_1;
    //LCD_1IN83_CS_0;
    DEV_SPI_WRITE(Data);
    //LCD_1IN83_CS_1;
}

/******************************************************************************
function :  send data
parameter:
    Data : Write data
******************************************************************************/
static void LCD_1IN83_SendData_16Bit(UWORD Data)
{
    LCD_1IN83_DC_1;
    LCD_1IN83_CS_0;
    DEV_SPI_WRITE((Data >> 8) & 0xFF);
    DEV_SPI_WRITE(Data & 0xFF);
    LCD_1IN83_CS_1;
}

/******************************************************************************
function :  Initialize the lcd register
parameter:
******************************************************************************/
static void LCD_1IN83_InitReg(void)
{
    LCD_1IN83_SendCommand(0xfd);
	LCD_1IN83_SendData_8Bit(0x06);
	LCD_1IN83_SendData_8Bit(0x08);

	LCD_1IN83_SendCommand(0x61);
	LCD_1IN83_SendData_8Bit(0x07);
	LCD_1IN83_SendData_8Bit(0x04);

	LCD_1IN83_SendCommand(0x62);
	LCD_1IN83_SendData_8Bit(0x00);
	LCD_1IN83_SendData_8Bit(0x44);
	LCD_1IN83_SendData_8Bit(0x45);

	LCD_1IN83_SendCommand(0x63);
	LCD_1IN83_SendData_8Bit(0x41);
	LCD_1IN83_SendData_8Bit(0x07);
	LCD_1IN83_SendData_8Bit(0x12);
	LCD_1IN83_SendData_8Bit(0x12);

	LCD_1IN83_SendCommand(0x64);
	LCD_1IN83_SendData_8Bit(0x37);
	//VSP
	LCD_1IN83_SendCommand(0x65);
	LCD_1IN83_SendData_8Bit(0x09);
	LCD_1IN83_SendData_8Bit(0x10);
	LCD_1IN83_SendData_8Bit(0x21);
	//VSN
	LCD_1IN83_SendCommand(0x66);
	LCD_1IN83_SendData_8Bit(0x09);
	LCD_1IN83_SendData_8Bit(0x10);
	LCD_1IN83_SendData_8Bit(0x21);
	//add source_neg_time
	LCD_1IN83_SendCommand(0x67);
	LCD_1IN83_SendData_8Bit(0x20);
	LCD_1IN83_SendData_8Bit(0x40);

	//gamma vap/van
	LCD_1IN83_SendCommand(0x68);
	LCD_1IN83_SendData_8Bit(0x90);
	LCD_1IN83_SendData_8Bit(0x4c);
	LCD_1IN83_SendData_8Bit(0x7C);
	LCD_1IN83_SendData_8Bit(0x66);

	LCD_1IN83_SendCommand(0xb1);
	LCD_1IN83_SendData_8Bit(0x0F);
	LCD_1IN83_SendData_8Bit(0x02);
	LCD_1IN83_SendData_8Bit(0x01);

	LCD_1IN83_SendCommand(0xB4);
	LCD_1IN83_SendData_8Bit(0x01);
	////porch
	LCD_1IN83_SendCommand(0xB5);
	LCD_1IN83_SendData_8Bit(0x02);
	LCD_1IN83_SendData_8Bit(0x02);
	LCD_1IN83_SendData_8Bit(0x0a);
	LCD_1IN83_SendData_8Bit(0x14);

	LCD_1IN83_SendCommand(0xB6);
	LCD_1IN83_SendData_8Bit(0x04);
	LCD_1IN83_SendData_8Bit(0x01);
	LCD_1IN83_SendData_8Bit(0x9f);
	LCD_1IN83_SendData_8Bit(0x00);
	LCD_1IN83_SendData_8Bit(0x02);
	////gamme sel
	LCD_1IN83_SendCommand(0xdf);
	LCD_1IN83_SendData_8Bit(0x11);
	////gamma_test1 A1#_wangly
	//3030b_gamma_new_
	//GAMMA---------------------------------/////////////

	//GAMMA---------------------------------/////////////
	LCD_1IN83_SendCommand(0xE2);
	LCD_1IN83_SendData_8Bit(0x13);//vrp0[5:0]	V0 13
	LCD_1IN83_SendData_8Bit(0x00);//vrp1[5:0]	V1
	LCD_1IN83_SendData_8Bit(0x00);//vrp2[5:0]	V2
	LCD_1IN83_SendData_8Bit(0x30);//vrp3[5:0]	V61
	LCD_1IN83_SendData_8Bit(0x33);//vrp4[5:0]	V62
	LCD_1IN83_SendData_8Bit(0x3f);//vrp5[5:0]	V63

	LCD_1IN83_SendCommand(0xE5);
	LCD_1IN83_SendData_8Bit(0x3f);//vrn0[5:0]	V63
	LCD_1IN83_SendData_8Bit(0x33);//vrn1[5:0]	V62
	LCD_1IN83_SendData_8Bit(0x30);//vrn2[5:0]	V61
	LCD_1IN83_SendData_8Bit(0x00);//vrn3[5:0]	V2
	LCD_1IN83_SendData_8Bit(0x00);//vrn4[5:0]	V1
	LCD_1IN83_SendData_8Bit(0x13);//vrn5[5:0]  V0 13

	LCD_1IN83_SendCommand(0xE1);
	LCD_1IN83_SendData_8Bit(0x00);//prp0[6:0]	V15
	LCD_1IN83_SendData_8Bit(0x57);//prp1[6:0]	V51

	LCD_1IN83_SendCommand(0xE4);
	LCD_1IN83_SendData_8Bit(0x58);//prn0[6:0]	V51 !!
	LCD_1IN83_SendData_8Bit(0x00);//prn1[6:0]  V15 !!

	LCD_1IN83_SendCommand(0xE0);
	LCD_1IN83_SendData_8Bit(0x01);//pkp0[4:0]	V3  !!
	LCD_1IN83_SendData_8Bit(0x03);//pkp1[4:0]	V7  !!
	LCD_1IN83_SendData_8Bit(0x0e);//pkp3[4:0]	V29 !!
	LCD_1IN83_SendData_8Bit(0x0e);//pkp4[4:0]	V37 !!
	LCD_1IN83_SendData_8Bit(0x0c);//pkp5[4:0]	V45 !!
	LCD_1IN83_SendData_8Bit(0x15);//pkp6[4:0]	V56 !!
	LCD_1IN83_SendData_8Bit(0x19);//pkp7[4:0]	V60 !!

	LCD_1IN83_SendCommand(0xE3);
	LCD_1IN83_SendData_8Bit(0x1a);//pkn0[4:0]	V60 !!
	LCD_1IN83_SendData_8Bit(0x16);//pkn1[4:0]	V56 !!
	LCD_1IN83_SendData_8Bit(0x0C);//pkn2[4:0]	V45 !!
	LCD_1IN83_SendData_8Bit(0x0f);//pkn3[4:0]	V37 !!
	LCD_1IN83_SendData_8Bit(0x0e);//pkn4[4:0]	V29 !!
	LCD_1IN83_SendData_8Bit(0x0d);//pkn5[4:0]	V21 !!
	LCD_1IN83_SendData_8Bit(0x02);//pkn6[4:0]	V7  !!
	LCD_1IN83_SendData_8Bit(0x01);//pkn7[4:0]	V3  !!

	LCD_1IN83_SendCommand(0xE6);
	LCD_1IN83_SendData_8Bit(0x00);
	LCD_1IN83_SendData_8Bit(0xff);//SC_EN_START[7:0] f0

	LCD_1IN83_SendCommand(0xE7);
	LCD_1IN83_SendData_8Bit(0x01);//CS_START[3:0] 01
	LCD_1IN83_SendData_8Bit(0x04);//scdt_inv_sel cs_vp_en
	LCD_1IN83_SendData_8Bit(0x03);//CS1_WIDTH[7:0] 12
	LCD_1IN83_SendData_8Bit(0x03);//CS2_WIDTH[7:0] 12
	LCD_1IN83_SendData_8Bit(0x00);//PREC_START[7:0] 06
	LCD_1IN83_SendData_8Bit(0x12);//PREC_WIDTH[7:0] 12

	LCD_1IN83_SendCommand(0xE8); //source
	LCD_1IN83_SendData_8Bit(0x00); //VCMP_OUT_EN 81-
	LCD_1IN83_SendData_8Bit(0x70); //chopper_sel[6:4]
	LCD_1IN83_SendData_8Bit(0x00); //gchopper_sel[6:4] 60
	////gate
	LCD_1IN83_SendCommand(0xEc);
	LCD_1IN83_SendData_8Bit(0x52);//52

	LCD_1IN83_SendCommand(0xF1);
	LCD_1IN83_SendData_8Bit(0x01);//te_pol tem_extend 00 01 03
	LCD_1IN83_SendData_8Bit(0x01);
	LCD_1IN83_SendData_8Bit(0x02);


	LCD_1IN83_SendCommand(0xF6);
	LCD_1IN83_SendData_8Bit(0x09);
	LCD_1IN83_SendData_8Bit(0x10);
	LCD_1IN83_SendData_8Bit(0x00);
	LCD_1IN83_SendData_8Bit(0x00);

	LCD_1IN83_SendCommand(0xfd);
	LCD_1IN83_SendData_8Bit(0xfa);
	LCD_1IN83_SendData_8Bit(0xfc);

	LCD_1IN83_SendCommand(0x3a);
	LCD_1IN83_SendData_8Bit(0x05);

	LCD_1IN83_SendCommand(0x35);
	LCD_1IN83_SendData_8Bit(0x00);


    LCD_1IN83_SendCommand(0x21);

    LCD_1IN83_SendCommand(0x11);
    DEV_Delay_ms(200);
    LCD_1IN83_SendCommand(0x29);
    DEV_Delay_ms(10);
}

/********************************************************************************
function:   Set the resolution and scanning method of the screen
parameter:
        Scan_dir:   Scan direction
********************************************************************************/
static void LCD_1IN83_SetAttributes(UBYTE Scan_dir)
{
    // Get the screen scan direction
    LCD_1IN83.SCAN_DIR = Scan_dir;
    UBYTE MemoryAccessReg = 0x08;

    // Get GRAM and LCD width and height
    if (Scan_dir == HORIZONTAL) {
        LCD_1IN83.HEIGHT = LCD_1IN83_WIDTH;
        LCD_1IN83.WIDTH = LCD_1IN83_HEIGHT;
        MemoryAccessReg = 0X78;
    }
    else {
        LCD_1IN83.HEIGHT = LCD_1IN83_HEIGHT;
        LCD_1IN83.WIDTH = LCD_1IN83_WIDTH;
        MemoryAccessReg = 0X08;
    }

    // Set the read / write scan direction of the frame memory
    LCD_1IN83_SendCommand(0x36); // MX, MY, RGB mode
    LCD_1IN83_SendData_8Bit(MemoryAccessReg); // 0x08 set RGB
}

/********************************************************************************
function :  Initialize the lcd
parameter:
********************************************************************************/
void LCD_1IN83_Init(UBYTE Scan_dir)
{
    // Hardware reset
    LCD_1IN83_Reset();

    // Set the resolution and scanning method of the screen
    LCD_1IN83_SetAttributes(Scan_dir);

    // Set the initialization register
    LCD_1IN83_InitReg();
}

/********************************************************************************
function:   Sets the start position and size of the display area
parameter:
        Xstart  :   X direction Start coordinates
        Ystart  :   Y direction Start coordinates
        Xend    :   X direction end coordinates
        Yend    :   Y direction end coordinates
********************************************************************************/
void LCD_1IN83_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    if (LCD_1IN83.SCAN_DIR == VERTICAL) {
				// set the X coordinates
        LCD_1IN83_SendCommand(0x2A);
        LCD_1IN83_SendData_8Bit(Xstart >> 8);
        LCD_1IN83_SendData_8Bit(Xstart);
        LCD_1IN83_SendData_8Bit((Xend-1) >> 8);
        LCD_1IN83_SendData_8Bit(Xend-1);

        // set the Y coordinates
        LCD_1IN83_SendCommand(0x2B);
        LCD_1IN83_SendData_8Bit((Ystart+20) >> 8);
        LCD_1IN83_SendData_8Bit(Ystart+20);
        LCD_1IN83_SendData_8Bit((Yend+20-1) >> 8);
        LCD_1IN83_SendData_8Bit(Yend+20-1);

    }
    else {
        // set the X coordinates
        LCD_1IN83_SendCommand(0x2A);
        LCD_1IN83_SendData_8Bit((Xstart+20) >> 8);
        LCD_1IN83_SendData_8Bit(Xstart+20);
        LCD_1IN83_SendData_8Bit((Xend+20-1) >> 8);
        LCD_1IN83_SendData_8Bit(Xend+20-1);

        // set the Y coordinates
        LCD_1IN83_SendCommand(0x2B);
        LCD_1IN83_SendData_8Bit(Ystart >> 8);
        LCD_1IN83_SendData_8Bit(Ystart);
        LCD_1IN83_SendData_8Bit((Yend-1) >> 8);
        LCD_1IN83_SendData_8Bit(Yend-1);
    }

    LCD_1IN83_SendCommand(0X2C);
}

void LCD_1IN83_SetWindows_Point(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    if (LCD_1IN83.SCAN_DIR == VERTICAL) {
				// set the X coordinates
        LCD_1IN83_SendCommand(0x2A);
        LCD_1IN83_SendData_8Bit(Xstart >> 8);
        LCD_1IN83_SendData_8Bit(Xstart);
        LCD_1IN83_SendData_8Bit((Xend) >> 8);
        LCD_1IN83_SendData_8Bit(Xend);

        // set the Y coordinates
        LCD_1IN83_SendCommand(0x2B);
        LCD_1IN83_SendData_8Bit((Ystart+20) >> 8);
        LCD_1IN83_SendData_8Bit(Ystart+20);
        LCD_1IN83_SendData_8Bit((Yend+20) >> 8);
        LCD_1IN83_SendData_8Bit(Yend+20);

    }
    else {
        // set the X coordinates
        LCD_1IN83_SendCommand(0x2A);
        LCD_1IN83_SendData_8Bit((Xstart+20) >> 8);
        LCD_1IN83_SendData_8Bit(Xstart+20);
        LCD_1IN83_SendData_8Bit((Xend+20) >> 8);
        LCD_1IN83_SendData_8Bit(Xend+20);

        // set the Y coordinates
        LCD_1IN83_SendCommand(0x2B);
        LCD_1IN83_SendData_8Bit(Ystart >> 8);
        LCD_1IN83_SendData_8Bit(Ystart);
        LCD_1IN83_SendData_8Bit((Yend) >> 8);
        LCD_1IN83_SendData_8Bit(Yend);
    }

    LCD_1IN83_SendCommand(0X2C);
}


/******************************************************************************
function :  Clear screen
parameter:
******************************************************************************/
void LCD_1IN83_Clear(UWORD Color)
{
    UWORD i,j;

    LCD_1IN83_SetWindows(0, 0, LCD_1IN83.WIDTH, LCD_1IN83.HEIGHT);
    DEV_Digital_Write(DEV_DC_PIN, 1);

    for(i = 0; i < LCD_1IN83_WIDTH; i++) {
        for(j = 0; j < LCD_1IN83_HEIGHT; j++) {
            DEV_SPI_WRITE((Color>>8)&0xff);
            DEV_SPI_WRITE(Color);
        }
     }
}

/******************************************************************************
function :  Sends the image buffer in RAM to displays
parameter:
******************************************************************************/
void LCD_1IN83_Display(UWORD *Image)
{
    UWORD i,j;

    LCD_1IN83_SetWindows(0, 0, LCD_1IN83.WIDTH, LCD_1IN83.HEIGHT);
    DEV_Digital_Write(DEV_DC_PIN, 1);

    for(i = 0; i < LCD_1IN83_WIDTH; i++) {
        for(j = 0; j < LCD_1IN83_HEIGHT; j++) {
            DEV_SPI_WRITE((*(Image+i*LCD_1IN83_HEIGHT+j)>>8)&0xff);
            DEV_SPI_WRITE(*(Image+i*LCD_1IN83_WIDTH+j));
        }
    }

}

void LCD_1IN83_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image)
{
    // display
    UDOUBLE Addr = 0;
    UWORD i,j;

    LCD_1IN83_SetWindows(Xstart, Ystart, Xend , Yend);
    LCD_1IN83_DC_1;

    for (i = Ystart; i < Yend - 1; i++) {
        Addr = Xstart + i * LCD_1IN83_WIDTH ;
        for(j=Xstart;j<Xend-1;j++) {
            DEV_SPI_WRITE((*(Image+Addr+j)>>8)&0xff);
            DEV_SPI_WRITE(*(Image+Addr+j));
        }
    }
}

void LCD_1IN83_DrawPoint(UWORD X, UWORD Y, UWORD Color)
{
	LCD_1IN83_SetWindows_Point(X, Y, X, Y);
    LCD_1IN83_SendData_16Bit(Color);
}

void LCD_1IN83_SetBackLight(UWORD Value)
{
    DEV_Set_PWM(Value);
}
