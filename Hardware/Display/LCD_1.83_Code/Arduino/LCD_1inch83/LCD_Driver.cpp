/*****************************************************************************
* | File        :   LCD_Driver.c
* | Author      :   Waveshare team
* | Function    :   Electronic paper driver
* | Info        :
*----------------
* | This version:   V1.0
* | Date        :   2023-03-15
* | Info        :   
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
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
    if(Value > 100)
        Value=100;
    DEV_Set_BL(DEV_BL_PIN, (Value * 2.55));
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
    DEV_Digital_Write(DEV_CS_PIN,1);
}

/******************************************************************************
function: 
    Common register initialization
******************************************************************************/
void LCD_Init(void)
{
    LCD_Reset();

    //************* Start Initial Sequence **********// 
    
    LCD_WriteReg(0x36);
    if (VERTICAL)
        LCD_WriteData_Byte(0x08);
    else
        LCD_WriteData_Byte(0x78);
		
    LCD_WriteReg(0xfd);
	LCD_WriteData_Byte(0x06);
	LCD_WriteData_Byte(0x08);

	LCD_WriteReg(0x61);
	LCD_WriteData_Byte(0x07);
	LCD_WriteData_Byte(0x04);

	LCD_WriteReg(0x62);
	LCD_WriteData_Byte(0x00);
	LCD_WriteData_Byte(0x44);
	LCD_WriteData_Byte(0x45);

	LCD_WriteReg(0x63);
	LCD_WriteData_Byte(0x41);
	LCD_WriteData_Byte(0x07);
	LCD_WriteData_Byte(0x12);
	LCD_WriteData_Byte(0x12);

	LCD_WriteReg(0x64);
	LCD_WriteData_Byte(0x37);
	//VSP
	LCD_WriteReg(0x65);
	LCD_WriteData_Byte(0x09);
	LCD_WriteData_Byte(0x10);
	LCD_WriteData_Byte(0x21);
	//VSN
	LCD_WriteReg(0x66); 
	LCD_WriteData_Byte(0x09); 
	LCD_WriteData_Byte(0x10); 
	LCD_WriteData_Byte(0x21);
	//add source_neg_time
	LCD_WriteReg(0x67);
	LCD_WriteData_Byte(0x20);
	LCD_WriteData_Byte(0x40);

	//gamma vap/van
	LCD_WriteReg(0x68);
	LCD_WriteData_Byte(0x90);
	LCD_WriteData_Byte(0x4c);
	LCD_WriteData_Byte(0x7C);
	LCD_WriteData_Byte(0x66);

	LCD_WriteReg(0xb1);
	LCD_WriteData_Byte(0x0F);
	LCD_WriteData_Byte(0x02);
	LCD_WriteData_Byte(0x01);

	LCD_WriteReg(0xB4);
	LCD_WriteData_Byte(0x01); 
	////porch
	LCD_WriteReg(0xB5);
	LCD_WriteData_Byte(0x02);
	LCD_WriteData_Byte(0x02);
	LCD_WriteData_Byte(0x0a);
	LCD_WriteData_Byte(0x14);

	LCD_WriteReg(0xB6);
	LCD_WriteData_Byte(0x04);
	LCD_WriteData_Byte(0x01);
	LCD_WriteData_Byte(0x9f);
	LCD_WriteData_Byte(0x00);
	LCD_WriteData_Byte(0x02);
	////gamme sel
	LCD_WriteReg(0xdf);
	LCD_WriteData_Byte(0x11);
	////gamma_test1 A1#_wangly
	//3030b_gamma_new_
	//GAMMA---------------------------------/////////////

	//GAMMA---------------------------------/////////////
	LCD_WriteReg(0xE2);	
	LCD_WriteData_Byte(0x13);//vrp0[5:0]	V0 13
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
	LCD_WriteData_Byte(0x13);//vrn5[5:0]  V0 13

	LCD_WriteReg(0xE1);	
	LCD_WriteData_Byte(0x00);//prp0[6:0]	V15
	LCD_WriteData_Byte(0x57);//prp1[6:0]	V51 

	LCD_WriteReg(0xE4);	
	LCD_WriteData_Byte(0x58);//prn0[6:0]	V51 !!
	LCD_WriteData_Byte(0x00);//prn1[6:0]  V15 !!

	LCD_WriteReg(0xE0);
	LCD_WriteData_Byte(0x01);//pkp0[4:0]	V3  !!
	LCD_WriteData_Byte(0x03);//pkp1[4:0]	V7  !!
	LCD_WriteData_Byte(0x0e);//pkp3[4:0]	V29 !!
	LCD_WriteData_Byte(0x0e);//pkp4[4:0]	V37 !!
	LCD_WriteData_Byte(0x0c);//pkp5[4:0]	V45 !!
	LCD_WriteData_Byte(0x15);//pkp6[4:0]	V56 !!
	LCD_WriteData_Byte(0x19);//pkp7[4:0]	V60 !!

	LCD_WriteReg(0xE3);	
	LCD_WriteData_Byte(0x1a);//pkn0[4:0]	V60 !! 
	LCD_WriteData_Byte(0x16);//pkn1[4:0]	V56 !!
	LCD_WriteData_Byte(0x0C);//pkn2[4:0]	V45 !!
	LCD_WriteData_Byte(0x0f);//pkn3[4:0]	V37 !!
	LCD_WriteData_Byte(0x0e);//pkn4[4:0]	V29 !!
	LCD_WriteData_Byte(0x0d);//pkn5[4:0]	V21 !!
	LCD_WriteData_Byte(0x02);//pkn6[4:0]	V7  !!
	LCD_WriteData_Byte(0x01);//pkn7[4:0]	V3  !!
    
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
	LCD_WriteData_Byte(0x09);
	LCD_WriteData_Byte(0x10);
	LCD_WriteData_Byte(0x00);
	LCD_WriteData_Byte(0x00);

	LCD_WriteReg(0xfd);
	LCD_WriteData_Byte(0xfa);
	LCD_WriteData_Byte(0xfc);

	LCD_WriteReg(0x3a);
	LCD_WriteData_Byte(0x05);

	LCD_WriteReg(0x35);
	LCD_WriteData_Byte(0x00);


    LCD_WriteReg(0x21);

    LCD_WriteReg(0x11);
    DEV_Delay_ms(200);
    LCD_WriteReg(0x29);
    DEV_Delay_ms(10);
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
  if (HORIZONTAL) {
        // set the X coordinates
        LCD_WriteReg(0x2A);
        LCD_WriteData_Byte((Xstart+20) >> 8);
        LCD_WriteData_Byte(Xstart+20);
        LCD_WriteData_Byte((Xend+20)>> 8);
        LCD_WriteData_Byte(Xend+20);
        
        // set the Y coordinates
        LCD_WriteReg(0x2B);
        LCD_WriteData_Byte(Ystart >> 8);
        LCD_WriteData_Byte(Ystart);
        LCD_WriteData_Byte((Yend) >> 8);
        LCD_WriteData_Byte(Yend);
    }
    else {
        // set the X coordinates
        LCD_WriteReg(0x2A);
        LCD_WriteData_Byte(Xstart >> 8);
        LCD_WriteData_Byte(Xstart);
        LCD_WriteData_Byte((Xend) >> 8);
        LCD_WriteData_Byte(Xend);
        // set the Y coordinates
        LCD_WriteReg(0x2B);
        LCD_WriteData_Byte((Ystart+20)>> 8);
        LCD_WriteData_Byte(Ystart+20);
        LCD_WriteData_Byte((Yend+20)>> 8);
        LCD_WriteData_Byte(Yend+20);
    }

    LCD_WriteReg(0X2C);
}

/******************************************************************************
function: Clear screen function, refresh the screen to a certain color
parameter :
    Color :   The color you want to clear all the screen
******************************************************************************/
void LCD_Clear(UWORD Color)
{
  UWORD i,j;    
  LCD_SetCursor(0, 0, LCD_WIDTH, LCD_HEIGHT);
    for(i=0; i<LCD_WIDTH; i++) {
        for(j=0; j<LCD_HEIGHT; j++) {
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
    LCD_SetCursor(Xstart, Ystart, Xend, Yend);
    for(i=Ystart; i<Yend; i++) {                                
        for(j=Xstart; j<Xend; j++) {
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
