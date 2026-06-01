// ---------------------------------------------------------------------------
// DoorService.cpp
// PRODUCER service — monitors a physical button (pin 18) and publishes
// DOOR_LOCKED / DOOR_UNLOCKED events to the event bus.
//
// Responsibilities:
//   • Poll GPIO pin 18 for button presses (active-LOW with INPUT_PULLUP)
//   • Toggle the doorLocked shared state on each press
//   • Publish a VehicleEvent so other services can react
//   • Debounce the button with a 200 ms delay to filter contact bounce
//
// This service is a PRODUCER only — it never reads from the queue.
// Other services decide what to do with door events; this service just
// reports what happened. That separation is a core SoA principle.
// ---------------------------------------------------------------------------

#include <Arduino.h>
#include "VehicleEvents.h"
#include "EventBus.h"

#define DOOR_BUTTON_PIN 18   // GPIO connected to momentary push button

// Global state defined here; extern'd in VehicleEvents.h
bool doorLocked    = true;
bool headlightsOn  = false;
int  cabinTemp     = 70;

void DoorService(void *parameter)
{
    // lastState tracks the previous reading so we can detect edges
    // (transition from HIGH → LOW = button just pressed).
    static bool lastState = HIGH;

    while (true)
    {
        bool currentState = digitalRead(DOOR_BUTTON_PIN);

        // Falling edge detected: button went from released (HIGH) to
        // pressed (LOW). INPUT_PULLUP means the pin is HIGH by default
        // and pulled LOW when the button connects it to ground.
        if (lastState == HIGH && currentState == LOW)
        {
            // Toggle the shared door state
            doorLocked = !doorLocked;

            // Build the event payload
            VehicleEvent event;
            event.type  = doorLocked ? DOOR_LOCKED : DOOR_UNLOCKED;
            event.value = 0;   // no extra data needed for door events

            // Publish to the bus. portMAX_DELAY means this task will
            // block indefinitely if the queue is full (rare in practice).
            xQueueSend(eventQueue, &event, portMAX_DELAY);

            // Debounce delay — ignore further presses for 200 ms.
            // vTaskDelay here is fine because FreeRTOS yields the CPU
            // to other tasks during the delay (not a busy-wait).
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }

        lastState = currentState;

        // Poll every 20 ms — fast enough to catch a button press
        // without burning CPU cycles on constant polling.
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}