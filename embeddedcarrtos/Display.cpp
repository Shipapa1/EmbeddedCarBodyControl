// ---------------------------------------------------------------------------
// Display.cpp
// Drives the Nokia 1.44" 128x128 V1.1 color TFT (ST7735 controller) via
// software SPI on: CS=33, RST=32, A0/DC=25, SDA/MOSI=26, SCK=27.
//
// Software SPI constructor order for Adafruit_ST7735 is:
//   Adafruit_ST7735(CS, DC, MOSI, SCLK, RST)
//
// INITR_144GREENTAB is the init sequence Adafruit's library uses
// specifically for the 1.44" 128x128 variant (as opposed to INITR_BLACKTAB
// /INITR_REDTAB used by the smaller 0.96"/1.8" 128x160 variants). If colors
// look shifted or the image is offset after upload, that's the first thing
// to try changing.
// ---------------------------------------------------------------------------

#include "Display.h"

#define TFT_CS   33
#define TFT_RST  32
#define TFT_DC   25
#define TFT_MOSI 26
#define TFT_SCLK 27

Adafruit_ST7735 display(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void Display_Init()
{
    display.initR(INITR_144GREENTAB);   // 1.44" 128x128 panel init sequence

    display.setRotation(0);
    display.fillScreen(ST77XX_BLACK);
    display.setTextWrap(false);

    display.setTextSize(1);
    display.setTextColor(ST77XX_WHITE);
    display.setCursor(0, 0);
    display.println("BCM Display");
    display.println("Initializing...");

    delay(500);
    display.fillScreen(ST77XX_BLACK);
}

void Display_ShowStatus(bool doorLocked, bool headlightsOn, int cabinTemp)
{
    display.fillScreen(ST77XX_BLACK);
    display.setCursor(0, 0);
    display.setTextSize(1);

    display.setTextColor(ST77XX_CYAN);
    display.println("--- BCM STATUS ---");
    display.println();

    display.setTextColor(doorLocked ? ST77XX_GREEN : ST77XX_RED);
    display.print("Door:  ");
    display.println(doorLocked ? "LOCKED" : "UNLOCK");

    display.setTextColor(headlightsOn ? ST77XX_YELLOW : ST77XX_WHITE);
    display.print("Light: ");
    display.println(headlightsOn ? "ON" : "OFF");

    display.setTextColor(ST77XX_WHITE);
    display.print("Temp:  ");
    display.print(cabinTemp);
    display.println(" F");
}