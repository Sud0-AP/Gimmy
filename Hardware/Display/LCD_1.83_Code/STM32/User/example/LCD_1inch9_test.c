#include "image.h"
#include "LCD_Test.h"
#include "LCD_1in9.h"
#include "DEV_Config.h"


void LCD_1in9_test()
{
    printf("LCD_1IN9_test Demo\r\n");
    DEV_Module_Init();

    printf("LCD_1IN9_ Init and Clear...\r\n");
    LCD_1IN9_SetBackLight(1000);
    LCD_1IN9_Init(VERTICAL);
    LCD_1IN9_Clear(WHITE);
    
#if 1
    printf("Paint_NewImage\r\n");
    Paint_NewImage(LCD_1IN9_WIDTH, LCD_1IN9_HEIGHT, 0, WHITE);

    printf("Set Clear and Display Funtion\r\n");
    Paint_SetClearFuntion(LCD_1IN9_Clear);
    Paint_SetDisplayFuntion(LCD_1IN9_DrawPoint);

    printf("Paint_Clear\r\n");
    Paint_Clear(WHITE);
    DEV_Delay_ms(100);

    printf("drawing...\r\n");
    Paint_SetRotate(180);
    Paint_DrawString_EN(30, 10, "123", &Font24, YELLOW, RED);  
    Paint_DrawString_EN(30, 34, "ABC", &Font24, BLUE, CYAN);
    
    Paint_DrawFloatNum (30, 58, 987.654321,3, &Font12, WHITE, BLACK);
    Paint_DrawString_CN(30, 130, "微雪电子", &Font24CN, WHITE, BLUE);
    
    Paint_DrawImage(gImage_1, 30, 70, 60, 60);

    Paint_DrawRectangle(185, 10, 285, 58, RED, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawLine(185, 10, 285, 58, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Paint_DrawLine(285, 10, 185, 58, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    
    Paint_DrawCircle(210, 100, 25, BLUE, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(240, 100, 25, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(280, 100, 25, RED, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(235, 125, 25, YELLOW, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(255, 125, 25, GREEN, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    
    DEV_Delay_ms(3000);
#endif


#if 1
    printf("Paint_NewImage\r\n");
    Paint_NewImage(LCD_1IN9_WIDTH, LCD_1IN9_HEIGHT, 90, WHITE);

    printf("Set Clear and Display Funtion\r\n");
    Paint_SetClearFuntion(LCD_1IN9_Clear);
    Paint_SetDisplayFuntion(LCD_1IN9_DrawPoint);

    printf("Paint_Clear\r\n");
    Paint_Clear(WHITE);
    DEV_Delay_ms(100);

    printf("drawing...\r\n");
    Paint_DrawString_EN(30, 10, "123", &Font24, YELLOW, RED);  
    Paint_DrawString_EN(30, 34, "ABC", &Font24, BLUE, CYAN);
    
    Paint_DrawFloatNum (30, 58, 987.654321,3, &Font12, WHITE, BLACK);
    Paint_DrawString_CN(30, 130, "微雪电子", &Font24CN, WHITE, BLUE);
    
    Paint_DrawImage(gImage_1, 30, 70, 60, 60);

    Paint_DrawRectangle(100, 20, 160, 120, RED, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawLine(100, 20, 160, 120, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Paint_DrawLine(100, 120, 160, 20, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    
    Paint_DrawCircle(50, 220, 25, BLUE, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(80, 220, 25, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(110, 220, 25, RED, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(65, 245, 25, YELLOW, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(95, 245, 25, GREEN, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    
    DEV_Delay_ms(3000);
#endif

    printf("quit...\r\n");
    DEV_Module_Exit();
}

