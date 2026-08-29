#include "DEV_Config.h"
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "fonts.h"

// Rotary encoder — temporary bring-up wiring, borrowing the I2C pins
// (expander isn't wired in yet; move these off D4/D5 once it is)
#define ENC_CLK D4
#define ENC_DT  D5

volatile int encoderCount = 0;
volatile uint8_t lastCLK = HIGH;

void IRAM_ATTR handleEncoder() {
  uint8_t clkState = digitalRead(ENC_CLK);
  if (clkState != lastCLK) {           // CLK just transitioned
    if (digitalRead(ENC_DT) != clkState) {
      encoderCount++;                  // clockwise
    } else {
      encoderCount--;                  // counter-clockwise
    }
  }
  lastCLK = clkState;
}

int lastDrawnCount = -999999;  // force the first draw

void setup() {
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);
  lastCLK = digitalRead(ENC_CLK);
  attachInterrupt(digitalPinToInterrupt(ENC_CLK), handleEncoder, CHANGE);

  Config_Init();
  LCD_Init();

  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_0, WHITE);
  Paint_Clear(WHITE);
  Paint_DrawString_EN(20, 20, "Encoder test", &Font20, WHITE, BLACK);
}

void loop() {
  if (encoderCount != lastDrawnCount) {
    Paint_ClearWindows(20, 60, 220, 100, WHITE);   // wipe just the number, not the whole screen
    Paint_DrawNum(20, 60, encoderCount, &Font24, WHITE, BLACK);
    lastDrawnCount = encoderCount;
  }
  delay(5);
}
