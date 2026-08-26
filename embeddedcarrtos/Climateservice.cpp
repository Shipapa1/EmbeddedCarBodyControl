// ---------------------------------------------------------------------------
// ClimateService.cpp
// SENSOR service — periodically reads a potentiometer on the ADC pin 34
// and maps its value to a simulated cabin temperature (60–90 °F).
//
// Responsibilities:
//   • Read the 12-bit ADC value from pin 34 every 1 second
//   • Map the raw 0–4095 ADC range to a 60–90 °F temperature range
//   • Update the cabinTemp shared state for Dashboard to display
//
// Why no queue here?
//   Temperature is a continuous sensor value, not a discrete event.
//   Flooding the queue with TEMP_UPDATE every second would waste queue
//   slots. Instead we write directly to the cabinTemp global and let
//   Dashboard read it on its own schedule.
//   In a production system you might use a mutex-protected struct or a
//   separate "data store" service instead of a raw global.
//
// ESP32 ADC notes:
//   - Pin 34 is input-only (no internal pull-up/down) — ideal for ADC.
//   - ESP32 ADC is 12-bit: values range 0–4095.
//   - ADC accuracy degrades above ~3.1 V; for a demo potentiometer this
//     is acceptable.
// ---------------------------------------------------------------------------

#include <Arduino.h>
#include "VehicleEvents.h"

#define TEMP_ADC_PIN 34     // Analog input — potentiometer wiper
#define TEMP_MIN     60     // Minimum simulated temperature (°F)
#define TEMP_MAX     90     // Maximum simulated temperature (°F)
#define ADC_MAX      4095   // 12-bit ADC ceiling on ESP32

void ClimateService(void *parameter)
{
    while (true)
    {
        // Read raw ADC (0–4095)
        int adc = analogRead(TEMP_ADC_PIN);

        // Arduino map() does linear interpolation:
        // cabinTemp = TEMP_MIN + (adc / ADC_MAX) * (TEMP_MAX - TEMP_MIN)
        cabinTemp = map(adc, 0, ADC_MAX, TEMP_MIN, TEMP_MAX);

        // Sleep for 1 second. portTICK_PERIOD_MS converts milliseconds
        // to FreeRTOS ticks (usually 1 tick = 1 ms on ESP32).
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}