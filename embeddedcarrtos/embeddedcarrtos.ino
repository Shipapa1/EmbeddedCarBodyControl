
// setup() runs once on boot:
//      - Initialize Serial for dashboard output
//      - Configure GPIO pins of the components on breadborad
//      - Initialize the event FreeRTOS queue
//      - Create the RTOS tasks
//
// loop() is left empty (rtos doesnt require the loop() )
//      Once xTaskCreatePinnedToCore() calls are made, FreeRTOS owns
//      scheduling. The Arduino loop() runs as a low-priority task that
//      yields immediately  leaving all CPU cycles for our services
//
// Task pinning strategy (dual-core ESP32):
//   Core 0: DoorService, ClimateService  — sensor/input tasks
//   Core 1: LightService, DashboardService — output/display tasks
//   Spreading tasks across cores prevents one slow task from starving
//   the others on the same core.
//
// Made Stack size (2048 bytes per task):
//   Should be sufficient for these lightweight tasks.
//   Used uxTaskGetStackHighWaterMark() to profile stack usage.
//
// Priority all set to 1:
//   Equal priority means the FreeRTOS scheduler round-robins between
//   tasks when multiple are ready. In production I'd assign higher
//   priority to safety-critical tasks.
// ---------------------------------------------------------------------------

#include <Arduino.h>
#include "VehicleEvents.h"
#include "EventBus.h"

// Forward declarations of .cpp files
void DoorService(void *parameter);
void LightButtonService(void *parameter);
void LightService(void *parameter);
void ClimateService(void *parameter);
void DashboardService(void *parameter);

// Called by EventBus.cpp
void EventBus_Init();

void setup()
{

    Serial.begin(115200);

    pinMode(18, INPUT_PULLUP);  // Door toggle button
    pinMode(19, INPUT_PULLUP);  // Dashboard status button
    pinMode(22, INPUT_PULLUP);  // Manual light toggle button
    pinMode(23, OUTPUT);        // Headlight LED

    EventBus_Init();


    // Create RTOS tasks
    //    xTaskCreatePinnedToCore(function, name, stack, param, priority,
    //                            handle, core)
    
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
        0                   // Core 0
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


// loop() is intentionally empty.
// FreeRTOS scheduler manages all execution
// ---------------------------------------------------------------------------
void loop()
{
}
