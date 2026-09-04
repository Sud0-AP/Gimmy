/*****************************************************************************
* | File        :   LCD_Driver.c
* | Author      :   Waveshare team
* | Function    :   Electronic paper driver
* | Info        :
*----------------
* | This version:   V1.0
* | Date        :   2018-11-18
* | Info        :   
#
# Permission is hereby granted, free of UBYTEge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "LCD_Driver.h"

/*******************************************************************************
function:
  Hardware reset
*******************************************************************************/
static void LCD_Reset(void)
{
  DEV_Digital_Write(DEV_CS_PIN,0);
  DEV_Delay_ms(20);
  DEV_Digital_Write(DEV_RST_PIN,0);
  DEV_Delay_ms(20);
  DEV_Digital_Write(DEV_RST_PIN,1);
  DEV_Delay_ms(20);
}

/*******************************************************************************
function:
  Setting backlight
parameter :
    value : Range 0~255   Duty cycle is value/255
*******************************************************************************/
void LCD_SetBacklight(UWORD Value)
{
  DEV_Set_BL(DEV_BL_PIN, Value);
}

/*******************************************************************************
function:
    Write register address and data
*******************************************************************************/
void LCD_WriteData_Byte(UBYTE da) 
{ 
  DEV_Digital_Write(DEV_CS_PIN,0);
  DEV_Digital_Write(DEV_DC_PIN,1);
  DEV_SPI_WRITE(da);  
  DEV_Digital_Write(DEV_CS_PIN,1);
}  

 void LCD_WriteData_Word(UWORD da)
{
  UBYTE i=(da>>8)&0xff;
  DEV_Digital_Write(DEV_CS_PIN,0);
  DEV_Digital_Write(DEV_DC_PIN,1);
  DEV_SPI_WRITE(i);
  DEV_SPI_WRITE(da);
  DEV_Digital_Write(DEV_CS_PIN,1);
}   

void LCD_WriteReg(UBYTE da)  
{ 
  DEV_Digital_Write(DEV_CS_PIN,0);
  DEV_Digital_Write(DEV_DC_PIN,0);
  DEV_SPI_WRITE(da);
  //DEV_Digital_Write(DEV_CS_PIN,1);
}

/******************************************************************************
function: 
    Common register initialization
******************************************************************************/
void LCD_Init(void)
{
  LCD_Reset();

  LCD_WriteReg(0x36);
	LCD_WriteData_Byte(0x00);

  //************* Start Initial Sequence **********// 
  LCD_WriteReg(0xfd);//private_access
	LCD_WriteData_Byte(0x06);
	LCD_WriteData_Byte(0x08);

	LCD_WriteReg(0x61);//add
	LCD_WriteData_Byte(0x07);//
	LCD_WriteData_Byte(0x04);//

	LCD_WriteReg(0x62);//bias setting
	LCD_WriteData_Byte(0x00);//00
	LCD_WriteData_Byte(0x44);//44
	LCD_WriteData_Byte(0x45);//40  47

	LCD_WriteReg(0x63);//
	LCD_WriteData_Byte(0x41);//
	LCD_WriteData_Byte(0x07);//
	LCD_WriteData_Byte(0x12);//
	LCD_WriteData_Byte(0x12);//

	LCD_WriteReg(0x64);//
	LCD_WriteData_Byte(0x37);//
	//VSP
	LCD_WriteReg(0x65);//Pump1=4.7MHz //PUMP1 VSP
	LCD_WriteData_Byte(0x09);//D6-5:pump1_clk[1:0] clamp 28 2b
	LCD_WriteData_Byte(0x10);//6.26
	LCD_WriteData_Byte(0x21);
	//VSN
	LCD_WriteReg(0x66); //pump=2 AVCL
	LCD_WriteData_Byte(0x09); //clamp 08 0b 09
	LCD_WriteData_Byte(0x10); //10
	LCD_WriteData_Byte(0x21);
	//add source_neg_time
	LCD_WriteReg(0x67);//pump_sel
	LCD_WriteData_Byte(0x21);//21 20
	LCD_WriteData_Byte(0x40);

	//gamma vap/van
	LCD_WriteReg(0x68);//gamma vap/van
	LCD_WriteData_Byte(0x90);//
	LCD_WriteData_Byte(0x4c);//
	LCD_WriteData_Byte(0x50);//VCOM  
	LCD_WriteData_Byte(0x70);//

	LCD_WriteReg(0xb1);//frame rate
	LCD_WriteData_Byte(0x0F);//0x0f fr_h[5:0] 0F
	LCD_WriteData_Byte(0x02);//0x02 fr_v[4:0] 02
	LCD_WriteData_Byte(0x01);//0x04 fr_div[2:0] 04

	LCD_WriteReg(0xB4);
	LCD_WriteData_Byte(0x01); //01:1dot 00:column
	////porch
	LCD_WriteReg(0xB5);
	LCD_WriteData_Byte(0x02);//0x02 vfp[6:0]
	LCD_WriteData_Byte(0x02);//0x02 vbp[6:0]
	LCD_WriteData_Byte(0x0a);//0x0A hfp[6:0]
	LCD_WriteData_Byte(0x14);//0x14 hbp[6:0]

	LCD_WriteReg(0xB6);
	LCD_WriteData_Byte(0x04);//
	LCD_WriteData_Byte(0x01);//
	LCD_WriteData_Byte(0x9f);//
	LCD_WriteData_Byte(0x00);//
	LCD_WriteData_Byte(0x02);//
	////gamme sel
	LCD_WriteReg(0xdf);//
	LCD_WriteData_Byte(0x11);//gofc_gamma_en_sel=1
	////gamma_test1 A1#_wangly
	//3030b_gamma_new_
	//GAMMA---------------------------------/////////////

	//GAMMA---------------------------------/////////////
	LCD_WriteReg(0xE2);	
	LCD_WriteData_Byte(0x03);//vrp0[5:0]	V0 03
	LCD_WriteData_Byte(0x00);//vrp1[5:0]	V1 
	LCD_WriteData_Byte(0x00);//vrp2[5:0]	V2 
	LCD_WriteData_Byte(0x30);//vrp3[5:0]	V61 
	LCD_WriteData_Byte(0x33);//vrp4[5:0]	V62 
	LCD_WriteData_Byte(0x3f);//vrp5[5:0]	V63

	LCD_WriteReg(0xE5);	
	LCD_WriteData_Byte(0x3f);//vrn0[5:0]	V63
	LCD_WriteData_Byte(0x33);//vrn1[5:0]	V62	
	LCD_WriteData_Byte(0x30);//vrn2[5:0]	V61 
	LCD_WriteData_Byte(0x00);//vrn3[5:0]	V2 
	LCD_WriteData_Byte(0x00);//vrn4[5:0]	V1 
	LCD_WriteData_Byte(0x03);//vrn5[5:0]  V0 03

	LCD_WriteReg(0xE1);	
	LCD_WriteData_Byte(0x05);//prp0[6:0]	V15
	LCD_WriteData_Byte(0x67);//prp1[6:0]	V51 

	LCD_WriteReg(0xE4);	
	LCD_WriteData_Byte(0x67);//prn0[6:0]	V51 
	LCD_WriteData_Byte(0x06);//prn1[6:0]  V15

	LCD_WriteReg(0xE0);
	LCD_WriteData_Byte(0x05);//pkp0[4:0]	V3 
	LCD_WriteData_Byte(0x06);//pkp1[4:0]	V7  
	LCD_WriteData_Byte(0x0A);//pkp2[4:0]	V21
	LCD_WriteData_Byte(0x0C);//pkp3[4:0]	V29 
	LCD_WriteData_Byte(0x0B);//pkp4[4:0]	V37 
	LCD_WriteData_Byte(0x0B);//pkp5[4:0]	V45 
	LCD_WriteData_Byte(0x13);//pkp6[4:0]	V56 
	LCD_WriteData_Byte(0x19);//pkp7[4:0]	V60 

	LCD_WriteReg(0xE3);	
	LCD_WriteData_Byte(0x18);//pkn0[4:0]	V60 
	LCD_WriteData_Byte(0x13);//pkn1[4:0]	V56 
	LCD_WriteData_Byte(0x0D);//pkn2[4:0]	V45 
	LCD_WriteData_Byte(0x09);//pkn3[4:0]	V37 
	LCD_WriteData_Byte(0x0B);//pkn4[4:0]	V29 
	LCD_WriteData_Byte(0x0B);//pkn5[4:0]	V21 
	LCD_WriteData_Byte(0x05);//pkn6[4:0]	V7  
	LCD_WriteData_Byte(0x06);//pkn7[4:0]	V3 
	//GAMMA---------------------------------/////////////

	//source
	LCD_WriteReg(0xE6);
	LCD_WriteData_Byte(0x00);
	LCD_WriteData_Byte(0xff);//SC_EN_START[7:0] f0

	LCD_WriteReg(0xE7);
	LCD_WriteData_Byte(0x01);//CS_START[3:0] 01
	LCD_WriteData_Byte(0x04);//scdt_inv_sel cs_vp_en
	LCD_WriteData_Byte(0x03);//CS1_WIDTH[7:0] 12
	LCD_WriteData_Byte(0x03);//CS2_WIDTH[7:0] 12
	LCD_WriteData_Byte(0x00);//PREC_START[7:0] 06
	LCD_WriteData_Byte(0x12);//PREC_WIDTH[7:0] 12

	LCD_WriteReg(0xE8); //source
	LCD_WriteData_Byte(0x00); //VCMP_OUT_EN 81-
	LCD_WriteData_Byte(0x70); //chopper_sel[6:4]
	LCD_WriteData_Byte(0x00); //gchopper_sel[6:4] 60
	////gate
	LCD_WriteReg(0xEc);
	LCD_WriteData_Byte(0x52);//52

	LCD_WriteReg(0xF1);
	LCD_WriteData_Byte(0x01);//te_pol tem_extend 00 01 03
	LCD_WriteData_Byte(0x01);
	LCD_WriteData_Byte(0x02);


	LCD_WriteReg(0xF6);
	LCD_WriteData_Byte(0x01);
	LCD_WriteData_Byte(0x30);
	LCD_WriteData_Byte(0x00);//
	LCD_WriteData_Byte(0x00);//40 3线2通道

	LCD_WriteReg(0xfd);
	LCD_WriteData_Byte(0xfa);
	LCD_WriteData_Byte(0xfc);

	LCD_WriteReg(0x3a);
	LCD_WriteData_Byte(0x55);//

	LCD_WriteReg(0x35);
	LCD_WriteData_Byte(0x00);

  

	LCD_WriteReg(0x21); 

	LCD_WriteReg(0x11);

	LCD_WriteReg(0x29);
} 

/******************************************************************************
function: Set the cursor position
parameter :
    Xstart:   Start UWORD x coordinate
    Ystart:   Start UWORD y coordinate
    Xend  :   End UWORD coordinates
    Yend  :   End UWORD coordinatesen
******************************************************************************/
void LCD_SetCursor(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD  Yend)
{ 
  Ystart = Ystart + 20;
	Yend = Yend + 20;
  LCD_WriteReg(0x2a);
  LCD_WriteData_Byte(Xstart>>8);
  LCD_WriteData_Byte(Xstart);
  LCD_WriteData_Byte(Xend >>8);
  LCD_WriteData_Byte(Xend );

  LCD_WriteReg(0x2b);
  LCD_WriteData_Byte(Ystart>>8);
  LCD_WriteData_Byte(Ystart & 0xFF);
  LCD_WriteData_Byte(Yend>>8);
  LCD_WriteData_Byte((Yend) & 0xFF);

  LCD_WriteReg(0x2C);
}

/******************************************************************************
function: Clear screen function, refresh the screen to a certain color
parameter :
    Color :   The color you want to clear all the screen
******************************************************************************/
void LCD_Clear(UWORD Color)
{
  UWORD i,j;    
  LCD_SetCursor(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);

  DEV_Digital_Write(DEV_DC_PIN,1);
  for(i = 0; i < LCD_WIDTH; i++){
    for(j = 0; j < LCD_HEIGHT; j++){
      LCD_WriteData_Word((Color>>8)&0xff);
      LCD_WriteData_Word(Color);
    }
   }
}

/******************************************************************************
function: Refresh a certain area to the same color
parameter :
    Xstart:   Start UWORD x coordinate
    Ystart:   Start UWORD y coordinate
    Xend  :   End UWORD coordinates
    Yend  :   End UWORD coordinates
    color :   Set the color
******************************************************************************/
void LCD_ClearWindow(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,UWORD color)
{          
  UWORD i,j; 
  LCD_SetCursor(Xstart, Ystart, Xend-1,Yend-1);
  for(i = Ystart; i <= Yend-1; i++){                                
    for(j = Xstart; j <= Xend-1; j++){
      LCD_WriteData_Word(color);
    }
  }                   
}

/******************************************************************************
function: Set the color of an area
parameter :
    Xstart:   Start UWORD x coordinate
    Ystart:   Start UWORD y coordinate
    Xend  :   End UWORD coordinates
    Yend  :   End UWORD coordinates
    Color :   Set the color
******************************************************************************/
void LCD_SetWindowColor(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,UWORD  Color)
{
  LCD_SetCursor( Xstart,Ystart,Xend,Yend);
  LCD_WriteData_Word(Color);      
}

/******************************************************************************
function: Draw a UWORD
parameter :
    X     :   Set the X coordinate
    Y     :   Set the Y coordinate
    Color :   Set the color
******************************************************************************/
void LCD_SetUWORD(UWORD x, UWORD y, UWORD Color)
{
  LCD_SetCursor(x,y,x,y);
  LCD_WriteData_Word(Color);      
} 
