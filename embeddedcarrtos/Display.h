#ifndef DISPLAY_H
#define DISPLAY_H

// ---------------------------------------------------------------------------
// Display.h
// Wraps the Nokia 1.44" 128x128 V1.1 color TFT (ST7735 controller) used to
// mirror the BCM dashboard status that is already printed to Serial.
//
// This module was originally coded for a PCD8544 (Nokia 5110, 84x48 mono)
// panel — wrong driver for this hardware. The ST7735 is a different
// controller with a different command set and library, even though the
// pin labels (VCC, GND, CS, RST, A0, SDA, SCK, LED) look identical.
//
// Wiring (unchanged from before):
//   VCC   -> 3.3V
//   GND   -> GND
//   CS    -> GPIO 33
//   RST   -> GPIO 32
//   A0/DC -> GPIO 25
//   SDA   -> GPIO 26  (MOSI / software SPI data)
//   SCK   -> GPIO 27  (software SPI clock)
//   LED   -> 3.3V through a ~100-330ohm resistor (backlight, not logic)
//
// Library required (Arduino Library Manager):
//   "Adafruit ST7735 and ST7789 Library" (depends on Adafruit GFX Library)
// ---------------------------------------------------------------------------

#include <Adafruit_ST7735.h>

// The one true instance of the display, defined in Display.cpp.
extern Adafruit_ST7735 display;

// Initializes SPI pins, panel init sequence, rotation, and clears the screen.
// Call once from setup(), after Serial.begin() and before the RTOS tasks
// that will draw to it are created.
void Display_Init();

// Redraws the BCM status snapshot on the LCD. Mirrors what DashboardService
// already prints to Serial. Safe to call repeatedly (it clears + redraws).
void Display_ShowStatus(bool doorLocked, bool headlightsOn, int cabinTemp);

#endif // DISPLAY_H