#ifndef VEHICLE_EVENTS_H
#define VEHICLE_EVENTS_H

// ---------------------------------------------------------------------------
// VehicleEvents.h
// Shared event definitions used across all services.
// This header is the "contract" — every service agrees on the same event
// types and payload format so they can communicate through the event bus
// without knowing anything about each other.
// ---------------------------------------------------------------------------

// Every possible event in the system gets an entry here.
// Using an enum keeps event types type-safe and readable vs. raw integers.
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

// ---------------------------------------------------------------------------
// Shared global state — declared extern so every .cpp file sees the same
// variables without creating duplicate copies.
// In a production SDV these would be accessed through a state-manager service,
// but for this demo extern globals are the simplest approach.
// ---------------------------------------------------------------------------
extern bool doorLocked;
extern bool headlightsOn;
extern int  cabinTemp;

#endif // VEHICLE_EVENTS_H