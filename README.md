# ESP32 Body Control Module (BCM) — FreeRTOS + SoA Demo
A Software-Defined Vehicle (SDV) body control simulation running on an ESP32
using FreeRTOS tasks and a Service-Oriented Architecture (SoA) event bus.
Demo Link: https://www.youtube.com/watch?v=f6kENB_romU
## Project Structure
```
BCM_Project/
│   ├── VehicleEvents.h   — Shared event types, struct, and extern globals
│   └── EventBus.h        — Queue handle declaration (middleware interface)
│   ├── middleware/
│   │   └── EventBus.cpp  — Queue definition and initialization
│   ├── services/
│   │   ├── DoorService.cpp      — Button input → publishes door events
│   │   ├── LightService.cpp     — Consumes door events → controls LED
│   │   ├── ClimateService.cpp   — ADC polling → updates cabin temp
│   │   └── DashboardService.cpp — Reads shared state → Serial output
│   └── embeddedcarrtos.cino          — setup(), GPIO config, task creation
```
## Hardware
| Component       | ESP32 Pin |
|----------------|-----------|
| Push button for Unlock/lock    | GPIO 18   |
| LED (headlight) | GPIO 23   |
| Potentiometer   | GPIO 34   |
| Push button for Status    | GPIO 19   |
| Push button for Light     | GPIO 22   |
## Architecture
```
[Button] → DoorService ──publish──► EventQueue (middleware)
                                          │
                                          └──consume──► LightService → [LED]
[Potentiometer] → ClimateService ──writes──► cabinTemp (shared state)
DashboardService ──reads──► doorLocked, headlightsOn, cabinTemp → Serial
```
### Key Design Decisions
- **SoA via FreeRTOS queue**: Services communicate through events, not direct function calls. Loose coupling means you can add/remove services without touching others.
- **Producer/Consumer separation**: DoorService only produces. LightService only consumes. Neither knows the other exists.
- **Dual-core task pinning**: Input tasks on Core 0, output tasks on Core 1 — balanced load across both ESP32 cores.
- **Blocking reads**: `xQueueReceive(portMAX_DELAY)` puts LightService to sleep until an event arrives — zero CPU wasted polling.
## Setup
1. Install Arduino IDE + ESP32 board package (Espressif)
2. Select **ESP32 Dev Module** from Tools → Board
3. Open `src/main.cpp` as the main sketch
4. Add remaining files as tabs (or use PlatformIO for proper folder support)
5. Upload and open Serial Monitor at **115200 baud**
## Schematic
<img width="986" height="522" alt="image" src="https://github.com/user-attachments/assets/98516682-0160-4db1-a240-32d1102c713c" />
