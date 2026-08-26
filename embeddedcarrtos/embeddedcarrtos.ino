// ---------------------------------------------------------------------------
// main.cpp
// Entry point for the ESP32 Body Control Module (BCM) demo.
//
// What happens here:
//   1. setup() runs once on boot:
//      a. Initialize Serial for dashboard output
//      b. Configure GPIO pins
//      c. Initialize the Nokia 5110 LCD (mirrors dashboard status)
//      d. Initialize the event bus (FreeRTOS queue)
//      e. Kick off the WiFi dashboard service (joins the motor ESP32's AP)
//      f. Spawn all RTOS tasks
//
//   2. loop() is intentionally empty.
//      Once xTaskCreatePinnedToCore() calls are made, FreeRTOS owns
//      scheduling. The Arduino loop() runs as a low-priority task that
//      yields immediately — leaving all CPU cycles for our services.
//
// Task pinning strategy (dual-core ESP32):
//   Core 0: DoorService, LightButtonService, ClimateService  — sensor/input
//   Core 1: LightService, DashboardService, WebDashboardService — output
//   Spreading tasks across cores prevents one slow task from starving
//   the others on the same core.
//
// Stack size:
//   2048 bytes for the lightweight sensor/button tasks. WebDashboardService
//   gets 4096 bytes since WiFi + WebServer + String-building need more
//   headroom than a simple GPIO poll loop.
//
// Priority (all set to 1):
//   Equal priority means the FreeRTOS scheduler round-robins between
//   tasks when multiple are ready. In production you'd assign higher
//   priority to safety-critical tasks (e.g., brake control > dashboard).
// ---------------------------------------------------------------------------

#include <Arduino.h>
#include "VehicleEvents.h"
#include "EventBus.h"
#include "Display.h"
#include "WebDashboardService.h"

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
    //
    //    NOTE: pins 9, 10, 11, 12, 13 are reserved for the Nokia LCD
    //    (see Display.cpp) and are configured internally by
    //    Adafruit_PCD8544::begin() — don't pinMode() them here.
    // -----------------------------------------------------------------------
    pinMode(18, INPUT_PULLUP);  // Door toggle button
    pinMode(19, INPUT_PULLUP);  // Dashboard status button
    pinMode(22, INPUT_PULLUP);  // Manual light toggle button
    pinMode(23, OUTPUT);        // Headlight LED

    // -----------------------------------------------------------------------
    // 3. Initialize the Nokia 5110 LCD (mirrors the dashboard status that
    //    is also printed to Serial). Done before task creation so the
    //    display is ready the moment DashboardService's first button
    //    press comes in.
    // -----------------------------------------------------------------------
    Display_Init();

    // -----------------------------------------------------------------------
    // 4. Initialize the event bus (creates the FreeRTOS queue)
    // -----------------------------------------------------------------------
    EventBus_Init();

    // -----------------------------------------------------------------------
    // 5. Kick off the WiFi dashboard service. This joins the motor ESP32's
    //    access point ("ESP32ForMotors") as a station. It's non-blocking —
    //    the actual connect/retry loop runs inside the WebDashboardService
    //    task below, so setup() won't hang if the motor board isn't powered
    //    on yet.
    // -----------------------------------------------------------------------
    WebDashboardService_Init();

    // -----------------------------------------------------------------------
    // 6. Create RTOS tasks
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

    xTaskCreatePinnedToCore(
        WebDashboardService,
        "WebDashboardService",
        4096,               // WiFi + WebServer need more stack than 2048
        NULL,
        1,
        NULL,
        1                   // Core 1 — it's an output/display task
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