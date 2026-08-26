// ---------------------------------------------------------------------------
// DashboardService.cpp
// DISPLAY service — prints the current BCM state to the Serial monitor
// AND to the Nokia 5110 LCD, only when the dashboard button (pin 19) is
// pressed.
//
// Responsibilities:
//   • Poll GPIO 19 for a button press (active-LOW, INPUT_PULLUP)
//   • On press: print a BCM status snapshot to Serial (unchanged)
//   • On press: draw the same snapshot to the Nokia LCD
//   • Debounce the button with a 200 ms delay
//
// Why on-demand instead of periodic?
//   A real vehicle HMI (instrument cluster, head unit) only updates the
//   display when the driver requests it or when a state change occurs —
//   not on a blind timer. This pattern is more realistic and also reduces
//   noise in the Serial monitor during development.
//
// Task affinity:
//   Pinned to Core 1. Polls every 20 ms — same strategy as DoorService.
// ---------------------------------------------------------------------------

#include <Arduino.h>
#include "VehicleEvents.h"
#include "Display.h"

#define DASHBOARD_BUTTON_PIN 19   // GPIO connected to dashboard button

void DashboardService(void *parameter)
{
    static bool lastState = HIGH;

    while (true)
    {
        bool currentState = digitalRead(DASHBOARD_BUTTON_PIN);

        // Falling edge: button just pressed
        if (lastState == HIGH && currentState == LOW)
        {
            // ---- Serial output (unchanged) ----
            Serial.println();
            Serial.println("========== BCM STATUS ==========");

            Serial.print("Door:   ");
            Serial.println(doorLocked ? "LOCKED" : "UNLOCKED");

            Serial.print("Lights: ");
            Serial.println(headlightsOn ? "ON" : "OFF");

            Serial.print("Temp:   ");
            Serial.print(cabinTemp);
            Serial.println(" F");

            Serial.println("================================");

            // ---- Nokia LCD output (new) ----
            Display_ShowStatus(doorLocked, headlightsOn, cabinTemp);

            // Debounce — ignore further presses for 200 ms
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }

        lastState = currentState;

        // Poll every 20 ms
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}