#ifndef EVENT_BUS_H
#define EVENT_BUS_H

// ---------------------------------------------------------------------------
// EventBus.h
// Declares the FreeRTOS queue that acts as the middleware message bus.
//
// In Service-Oriented Architecture (SoA), services should NOT call each other
// directly — that creates tight coupling. Instead, they publish events to a
// shared bus and subscribe to events they care about.
//
// Here the "bus" is a FreeRTOS QueueHandle_t:
//   - Producers call xQueueSend()    → put an event on the bus
//   - Consumers call xQueueReceive() → pull events off the bus
//
// The queue is defined once in EventBus.cpp and exposed here via extern
// so every service can access the same queue handle.
// ---------------------------------------------------------------------------

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

extern QueueHandle_t eventQueue;

#endif // EVENT_BUS_H