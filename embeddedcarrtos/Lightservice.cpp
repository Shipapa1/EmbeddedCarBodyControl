// ---------------------------------------------------------------------------
// LightService.cpp
// CONSUMER service — blocks on the event queue and controls the headlight
// LED (pin 23) in response to door events AND manual light button events.
//
// Responsibilities:
//   • Wait (block) for any event to arrive on the event bus
//   • React to DOOR_UNLOCKED → turn LED on
//   • React to DOOR_LOCKED   → turn LED off
//   • React to LIGHT_ON      → turn LED on  (manual override)
//   • React to LIGHT_OFF     → turn LED off (manual override)
//   • Update headlightsOn shared state so Dashboard reflects reality
//
// Two sources, one handler:
//   Both DoorService and LightButtonService publish to the same queue.
//   LightService doesn't care which button was pressed — it just reacts
//   to the event type. Adding a new light source in the future means
//   publishing the right event; this file doesn't need to change.
// ---------------------------------------------------------------------------

#include <Arduino.h>
#include "VehicleEvents.h"
#include "EventBus.h"

#define HEADLIGHT_PIN 23

void LightService(void *parameter)
{
    VehicleEvent event;

    while (true)
    {
        if (xQueueReceive(eventQueue, &event, portMAX_DELAY))
        {
            switch (event.type)
            {
                case DOOR_UNLOCKED:
                case LIGHT_ON:
                    headlightsOn = true;
                    digitalWrite(HEADLIGHT_PIN, HIGH);
                    break;

                case DOOR_LOCKED:
                case LIGHT_OFF:
                    headlightsOn = false;
                    digitalWrite(HEADLIGHT_PIN, LOW);
                    break;

                default:
                    // Ignore TEMP_UPDATE and any future event types
                    break;
            }
        }
    }
}