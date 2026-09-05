#include <Wire.h>
#include "DEV_Config.h"
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "fonts.h"

// Encoder — now on D6/D7 after moving off the I2C pins
#define ENC_CLK D6
#define ENC_DT  D7

// I2C expander
#define I2C_SDA D4
#define I2C_SCL D5
#define PCF8574_ADDR 0x20   // all 3 address pins low; adjust if yours differ

volatile int encoderCount = 0;
volatile uint8_t lastCLK = HIGH;

void IRAM_ATTR handleEncoder() {
  uint8_t clkState = digitalRead(ENC_CLK);
  if (clkState != lastCLK) {
    if (digitalRead(ENC_DT) != clkState) encoderCount++;
    else encoderCount--;
  }
  lastCLK = clkState;
}

uint8_t readExpander() {
  Wire.requestFrom(PCF8574_ADDR, 1);
  if (Wire.available()) return Wire.read();
  return 0xFF; // fail-safe: read failed, assume all-released
}

int lastEncoderShown = -999999;
uint8_t lastExpanderState = 0xFF;
char lastEventLine[32] = "";

void updateDisplay(bool redrawEncoder, bool redrawEvent) {
  if (redrawEncoder) {
    Paint_ClearWindows(10, 40, 270, 60, WHITE);
    char buf[32];
    snprintf(buf, sizeof(buf), "Encoder: %d", encoderCount);
    Paint_DrawString_EN(10, 40, buf, &Font16, WHITE, BLACK);
  }
  if (redrawEvent) {
    Paint_ClearWindows(10, 70, 270, 90, WHITE);
    Paint_DrawString_EN(10, 70, lastEventLine, &Font16, WHITE, BLACK);
  }
}

void setup() {
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);
  lastCLK = digitalRead(ENC_CLK);
  attachInterrupt(digitalPinToInterrupt(ENC_CLK), handleEncoder, CHANGE);

  Wire.begin(I2C_SDA, I2C_SCL);
  lastExpanderState = readExpander();  // capture idle state before comparing

  Config_Init();
  LCD_Init();
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_90, WHITE);
  Paint_Clear(WHITE);
  Paint_DrawString_EN(10, 10, "Input test", &Font16, WHITE, BLACK);
  updateDisplay(true, true);
}

void loop() {
  if (encoderCount != lastEncoderShown) {
    lastEncoderShown = encoderCount;
    updateDisplay(true, false);
  }

  uint8_t state = readExpander();
  if (state != lastExpanderState) {
    for (int i = 0; i < 8; i++) {
      bool wasPressed = !(lastExpanderState & (1 << i));
      bool isPressed  = !(state & (1 << i));
      if (isPressed && !wasPressed) {
        snprintf(lastEventLine, sizeof(lastEventLine), "P%d pressed", i);
        updateDisplay(false, true);
      } else if (!isPressed && wasPressed) {
        snprintf(lastEventLine, sizeof(lastEventLine), "P%d released", i);
        updateDisplay(false, true);
      }
    }
    lastExpanderState = state;
  }

  delay(20);  // crude polling interval — fine for this smoke test, not final debounce logic
}
