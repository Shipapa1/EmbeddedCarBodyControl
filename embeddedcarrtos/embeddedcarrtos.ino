// ---------------------------------------------------------------------------
// main.cpp
// Entry point for the ESP32 Body Control Module (BCM) demo.
//
// What happens here:
//   1. setup() runs once on boot:
//      a. Initialize Serial for dashboard output
//      b. Configure GPIO pins
//      c. Initialize the event bus (FreeRTOS queue)
//      d. Spawn all four RTOS tasks
//
//   2. loop() is intentionally empty.
//      Once xTaskCreatePinnedToCore() calls are made, FreeRTOS owns
//      scheduling. The Arduino loop() runs as a low-priority task that
//      yields immediately — leaving all CPU cycles for our services.c:\Users\myduy\OneDrive\Documents\Arduino\embeddedcarrtos
//
// Task pinning strategy (dual-core ESP32):
//   Core 0: DoorService, ClimateService  — sensor/input tasks
//   Core 1: LightService, DashboardService — output/display tasks
//   Spreading tasks across cores prevents one slow task from starving
//   the others on the same core.
//
// Stack size (2048 bytes per task):
//   Sufficient for these lightweight tasks. If a task calls deep
//   functions or uses large local buffers, increase accordingly.
//   Use uxTaskGetStackHighWaterMark() to profile stack usage.
//
// Priority (all set to 1):
//   Equal priority means the FreeRTOS scheduler round-robins between
//   tasks when multiple are ready. In production you'd assign higher
//   priority to safety-critical tasks (e.g., brake control > dashboard).
// ---------------------------------------------------------------------------

#include <Arduino.h>
#include "VehicleEvents.h"
#include "EventBus.h"

// Forward declarations — implementations live in their respective .cpp files
void DoorService(void *parameter);
void LightButtonService(void *parameter);
void LightService(void *parameter);
void ClimateService(void *parameter);
void DashboardService(void *parameter);

// Called by EventBus.cpp
void EventBus_Init();

void setup()
{
    // -----------------------------------------------------------------------
    // 1. Serial — 115200 baud matches Arduino IDE Serial Monitor default
    // -----------------------------------------------------------------------
    Serial.begin(115200);

    // -----------------------------------------------------------------------
    // 2. GPIO configuration
    //    Pin 18: button input with internal pull-up resistor enabled.
    //            Pressing the button pulls pin LOW (active-LOW logic).
    //    Pin 23: LED output for headlight simulation.
    // -----------------------------------------------------------------------
    pinMode(18, INPUT_PULLUP);  // Door toggle button
    pinMode(19, INPUT_PULLUP);  // Dashboard status button
    pinMode(22, INPUT_PULLUP);  // Manual light toggle button
    pinMode(23, OUTPUT);        // Headlight LED

    // -----------------------------------------------------------------------
    // 3. Initialize the event bus (creates the FreeRTOS queue)
    // -----------------------------------------------------------------------
    EventBus_Init();

    // -----------------------------------------------------------------------
    // 4. Create RTOS tasks
    //    xTaskCreatePinnedToCore(function, name, stack, param, priority,
    //                            handle, core)
    // -----------------------------------------------------------------------
    xTaskCreatePinnedToCore(
        DoorService,        // Task function
        "DoorService",      // Name (for debugging with vTaskList())
        2048,               // Stack size in bytes
        NULL,               // Parameter passed to task (unused)
        1,                  // Priority
        NULL,               // Task handle (unused here)
        0                   // Core 0
    );

    xTaskCreatePinnedToCore(
        LightButtonService,
        "LightButtonService",
        2048,
        NULL,
        1,
        NULL,
        0                   // Core 0 — alongside other input tasks
    );

    xTaskCreatePinnedToCore(
        LightService,
        "LightService",
        2048,
        NULL,
        1,
        NULL,
        1                   // Core 1
    );

    xTaskCreatePinnedToCore(
        ClimateService,
        "ClimateService",
        2048,
        NULL,
        1,
        NULL,
        0                   // Core 0
    );

    xTaskCreatePinnedToCore(
        DashboardService,
        "DashboardService",
        2048,
        NULL,
        1,
        NULL,
        1                   // Core 1
    );

    Serial.println("[BCM] All services started.");
}

// ---------------------------------------------------------------------------
// loop() is intentionally empty.
// FreeRTOS scheduler manages all execution from this point forward.
// ---------------------------------------------------------------------------
void loop()
{
}