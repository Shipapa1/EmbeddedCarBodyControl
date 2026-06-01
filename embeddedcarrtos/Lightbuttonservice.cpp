// PRODUCER service — monitors a dedicated light toggle button pin 22
// and publishes LIGHT_ON / LIGHT_OFF events to the event bus.
//
// Responsibilities:
//   -Poll GPIO 22 every 20 ms for a button press (active-LOW, INPUT_PULLUP)
//   -Toggle the headlightsOn shared state on each press
//   -Publish LIGHT_ON or LIGHT_OFF so LightService can drive the LED
//   -Debounce with a 200 ms delay
//
// Why a separate service instead of adding this to DoorService?
//   Single Responsibility Principle — each service owns exactly one input
//   source. DoorService owns the door button; this service owns the light
//   button. 

// This service is a PRODUCER only it never reads from the queue.
// LightService decides what to do with the events; this service just
// reports what the driver pressed.


#include <Arduino.h>
#include "VehicleEvents.h"
#include "EventBus.h"

#define LIGHT_BUTTON_PIN 22   

void LightButtonService(void *parameter)
{
    static bool lastState = HIGH;

    while (true)
    {
        bool currentState = digitalRead(LIGHT_BUTTON_PIN);

        // Falling edge: button just pressed
        if (lastState == HIGH && currentState == LOW)
        {
            // Toggle light state manually
            headlightsOn = !headlightsOn;

            VehicleEvent event;
            event.type  = headlightsOn ? LIGHT_ON : LIGHT_OFF;
            event.value = 0;

            xQueueSend(eventQueue, &event, portMAX_DELAY);

            // Debounce
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }

        lastState = currentState;

        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
