#ifndef VEHICLE_EVENTS_H
#define VEHICLE_EVENTS_H

// Shared event definitions used across all services.
// types and payload format so they can communicate through the event bus


// Every possible event of system gets an entry here.

enum EventType {
    DOOR_LOCKED,
    DOOR_UNLOCKED,
    LIGHT_ON,
    LIGHT_OFF,
    TEMP_UPDATE
};

// The message that travels through the queue (event bus).
// type  → what happened
// value → optional payload (e.g. a temperature reading, error code, etc.)
typedef struct {
    EventType type;
    int       value;
} VehicleEvent;


// Shared global state — declared extern so every .cpp file sees the same
// variables without creating duplicate copies.

extern bool doorLocked;
extern bool headlightsOn;
extern int  cabinTemp;

#endif // VEHICLE_EVENTS_H
