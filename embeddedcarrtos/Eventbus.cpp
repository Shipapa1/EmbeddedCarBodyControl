// ---------------------------------------------------------------------------
// EventBus.cpp
// Defines and initializes the FreeRTOS queue that acts as the shared
// middleware message bus for the Body Control Module (BCM).
//
// Why a queue?
//   A FreeRTOS queue is thread-safe by design. Multiple RTOS tasks can
//   call xQueueSend / xQueueReceive concurrently — the kernel handles
//   mutual exclusion internally. This is far safer than sharing a plain
//   global variable, which would require manual mutex protection.
//
// Queue depth (20):
//   Allows up to 20 unread VehicleEvent structs to queue up before the
//   sender blocks. Sized for demo purposes; tune based on burst rate in
//   real applications.
// ---------------------------------------------------------------------------

#include "EventBus.h"
#include "VehicleEvents.h"

// The one true instance of the event queue — every service imports this
// via the extern declaration in EventBus.h.
QueueHandle_t eventQueue;

void EventBus_Init()
{
    eventQueue = xQueueCreate(
        20,                   // max 20 events buffered at once
        sizeof(VehicleEvent)  // each slot holds one VehicleEvent struct
    );
}