# ESP32 Body Control Module (BCM) — FreeRTOS + SoA Demo

A Software-Defined Vehicle (SDV) body control simulation running on an ESP32
using FreeRTOS tasks and a Service-Oriented Architecture (SoA) event bus,
paired with a second ESP32 that drives the car's motors over WiFi.

Demo Link: https://www.youtube.com/watch?v=f6kENB_romU

## System Overview

This project uses **two ESP32 boards**:

| Board | Role | Sketch |
|---|---|---|
| **ESP32 #1 — BCM** | Runs the FreeRTOS body-control services (door, lights, climate, dashboard) and serves a live status dashboard over WiFi | `embeddedcarrtos.ino` |
| **ESP32 #2 — Motor Controller** | Hosts the shared WiFi access point and drives the car's drive motors from a browser-based control page | `MotorControl.ino` |

The Motor Controller ESP32 hosts a WiFi access point (`ESP32ForMotors`). The
BCM ESP32 joins that same network as a station with a fixed IP, so both
boards — and your phone or laptop — all sit on one WiFi network:

| Device | IP Address | Purpose |
|---|---|---|
| Motor Controller ESP32 (AP host) | `192.168.4.1` | Drive controls (forward/back/left/right) |
| BCM ESP32 (station) | `192.168.4.22` | Live BCM status dashboard |

**Both boards must be powered on for the system to work.** The Motor
Controller ESP32 hosts the network itself — if it's off, there is no WiFi
network for the BCM board (or your phone) to join. Power-on order:

1. Power on the **Motor Controller ESP32** first and wait for it to print
   `WiFi AP Started` in Serial Monitor.
2. Power on the **BCM ESP32** — it joins the network automatically.
3. Connect your phone/laptop to the `ESP32ForMotors` WiFi network to view
   either page.

## Project Structure

```
BCM_Project/
│   ├── VehicleEvents.h        — Shared event types, struct, and extern globals
│   └── EventBus.h             — Queue handle declaration (middleware interface)
│   ├── middleware/
│   │   └── EventBus.cpp       — Queue definition and initialization
│   ├── services/
│   │   ├── DoorService.cpp        — Button input → publishes door events
│   │   ├── LightService.cpp       — Consumes door events → controls LED
│   │   ├── LightButtonService.cpp — Manual light button → publishes light events
│   │   ├── ClimateService.cpp     — ADC polling → updates cabin temp
│   │   ├── DashboardService.cpp   — Reads shared state → Serial + LCD output
│   │   └── WebDashboardService.cpp — Joins WiFi, serves live status dashboard
│   ├── Display.h / Display.cpp     — Nokia 1.44" ST7735 LCD driver
│   └── embeddedcarrtos.ino         — setup(), GPIO config, task creation

MotorController_Project/
│   └── MotorControl.ino       — WiFi AP host + drive controls + link to BCM dashboard
```

## Hardware

### BCM ESP32

| Component       | ESP32 Pin |
|----------------|-----------|
| Push button for Unlock/lock    | GPIO 18   |
| LED (headlight) | GPIO 23   |
| Potentiometer   | GPIO 34   |
| Push button for Status    | GPIO 19   |
| Push button for Light     | GPIO 22   |

### Motor Controller ESP32

| Component       | ESP32 Pin |
|----------------|-----------|
| Motor driver IN1 | GPIO 26 |
| Motor driver IN2 | GPIO 25 |
| Motor driver IN3 | GPIO 33 |
| Motor driver IN4 | GPIO 32 |

## Architecture

```
[Button] → DoorService ──publish──► EventQueue (middleware)
                                          │
                                          └──consume──► LightService → [LED]

[Potentiometer] → ClimateService ──writes──► cabinTemp (shared state)

DashboardService ──reads──► doorLocked, headlightsOn, cabinTemp → Serial + LCD

WebDashboardService ──reads──► doorLocked, headlightsOn, cabinTemp
                     ──serves──► http://192.168.4.22/ (browser dashboard)

Motor Controller ESP32 ──hosts AP──► WiFi network "ESP32ForMotors"
                        ──serves──► http://192.168.4.1/ (drive controls)
```

### Key Design Decisions

- **SoA via FreeRTOS queue**: Services communicate through events, not direct function calls. Loose coupling means you can add/remove services without touching others.
- **Producer/Consumer separation**: DoorService only produces. LightService only consumes. Neither knows the other exists.
- **Dual-core task pinning**: Input tasks on Core 0, output tasks on Core 1 — balanced load across both ESP32 cores.
- **Blocking reads**: `xQueueReceive(portMAX_DELAY)` puts LightService to sleep until an event arrives — zero CPU wasted polling.
- **One AP, two boards**: rather than each ESP32 hosting its own network, the Motor Controller hosts a single WiFi access point that the BCM board joins as a station. This keeps both dashboards reachable from one WiFi connection instead of forcing users to swap networks.
- **Static IP on the BCM station**: the ESP32 softAP's DHCP server assigns addresses in join order, so without a fixed IP the BCM board could land on a different address every boot. `WebDashboardService` assigns itself `192.168.4.22` explicitly.
- **Non-blocking WiFi connect**: `WebDashboardService` doesn't block `setup()` waiting on a connection. It kicks off the join and retries in the background, so the BCM board's other services (door, lights, climate) start immediately even if the Motor Controller ESP32 isn't powered on yet.

## Setup

### BCM ESP32
1. Install Arduino IDE + ESP32 board package (Espressif)
2. Select **ESP32 Dev Module** from Tools → Board
3. Open `embeddedcarrtos.ino` as the main sketch
4. Add remaining `.cpp`/`.h` files as tabs (or use PlatformIO for proper folder support)
5. Upload and open Serial Monitor at **115200 baud**

### Motor Controller ESP32
1. Open `MotorControl.ino` as its own sketch (separate board, separate upload)
2. Select **ESP32 Dev Module** from Tools → Board
3. Upload and open Serial Monitor at **115200 baud** — confirm it prints `WiFi AP Started`
4. Power this board on **before** the BCM board (see boot order above)

## Schematic
<img width="986" height="522" alt="image" src="https://github.com/user-attachments/assets/98516682-0160-4db1-a240-32d1102c713c" />

## Test Cases

| Test | Action | Expected |
|------|--------|----------|
| 1 | Press door button | Door: UNLOCKED, LOCKED |
| 2 | Press light button | LED: On/OFF |
| 3 | Turn potentiometer | Temp changes (60–90 °F) |
| 4 | Press status button | Current car status on LCD |
| 5 | Visit `192.168.4.1` on phone/laptop | Drive controls page loads, motors respond |
| 6 | Visit `192.168.4.22` on phone/laptop | Live BCM dashboard loads and auto-updates every second |
