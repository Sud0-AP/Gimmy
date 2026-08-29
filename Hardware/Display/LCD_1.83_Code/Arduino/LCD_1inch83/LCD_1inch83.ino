#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "image.h"

void setup()
{
    Config_Init();
    LCD_Init();
    LCD_SetBacklight(100);
}


void loop()
{
#if 1
    Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, WHITE);
    Paint_Clear(WHITE);

    Paint_DrawString_EN(30, 10, "123", &Font24, YELLOW, RED);  
    Paint_DrawString_EN(30, 34, "ABC", &Font24, BLUE, CYAN);
    
    Paint_DrawString_CN(235,10, "微", &Font24CN, WHITE, RED);
    Paint_DrawString_CN(235,40, "雪", &Font24CN, WHITE, RED);
    Paint_DrawString_CN(235,70, "电", &Font24CN, WHITE, RED);
    Paint_DrawString_CN(235,100, "子", &Font24CN, WHITE, RED);

    Paint_DrawRectangle(125, 10, 225, 58, RED,DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawLine(125, 10, 225, 58, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Paint_DrawLine(225, 10, 125, 58, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Paint_DrawCircle(150, 100, 25, BLUE, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(180, 100, 25, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(210, 100, 25, RED, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(165, 125, 25, YELLOW, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(195, 125, 25, GREEN, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);

    Paint_DrawImage(gImage_70X70, 20, 80, 70, 70); 
    delay(3000);
#endif
        
#if 1
    Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, WHITE);
    Paint_Clear(WHITE);
    
    Paint_DrawString_EN(30, 10, "123", &Font24, YELLOW, RED);  
    Paint_DrawString_EN(30, 34, "ABC", &Font24, BLUE, CYAN);
    
    Paint_DrawString_CN(10, 150, "微", &Font24CN,WHITE, RED);
    Paint_DrawString_CN(45, 150, "雪", &Font24CN,WHITE, RED);
    Paint_DrawString_CN(80, 150, "电", &Font24CN,WHITE, RED);
    Paint_DrawString_CN(115, 150, "子", &Font24CN,WHITE, RED);
        
    Paint_DrawImage(gImage_70X70, 10, 70, 70, 70);
    
    Paint_DrawRectangle(100, 20, 160, 120, RED, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawLine(100, 20, 160, 120, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Paint_DrawLine(100, 120, 160, 20, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    
    Paint_DrawCircle(50, 220, 25, BLUE, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(80, 220, 25, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(110, 220, 25, RED, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(65, 245, 25, YELLOW, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(95, 245, 25, GREEN, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    
    delay(3000);
#endif
}



/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
