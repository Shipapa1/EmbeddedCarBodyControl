// ---------------------------------------------------------------------------
// WebDashboardService.cpp
// Joins the car's existing WiFi network (hosted by the motor-control ESP32)
// as a station, then serves a live BCM status dashboard over HTTP.
//
// Why station mode instead of a second Access Point?
//   Two ESP32s can't both host the same AP. The motor ESP32 already creates
//   "ESP32ForMotors" as an access point, so the BCM ESP32 joins that network
//   as a client, same as your phone or laptop would. Everything then shares
//   one WiFi network:
//     Motor ESP32 (AP host):  192.168.4.1   -> /F /B /L /R /S drive controls
//     BCM ESP32   (station):  192.168.4.22  -> /  and /status dashboard
//
//   The BCM ESP32 is given a FIXED static IP (192.168.4.22) instead of
//   relying on DHCP. The ESP32 softAP's built-in DHCP server hands out
//   addresses in join order, so without a static IP the BCM board could
//   land on a different address every boot (especially once you also
//   connect a phone/laptop to the same network to view the pages).
//
// Endpoints:
//   GET /        -> HTML dashboard page (auto-refreshes via JS, no reload)
//   GET /status  -> JSON snapshot of doorLocked / headlightsOn / cabinTemp
//
// Non-blocking connect:
//   setup() should not hang forever if the motor ESP32 (and therefore the
//   WiFi network) isn't powered on yet. WebDashboardService_Init() kicks
//   off the connection and returns immediately; the WebDashboardService
//   task itself watches WiFi.status() and retries the connection in the
//   background, serving pages once it succeeds.
// ---------------------------------------------------------------------------

#include <WiFi.h>
#include <WebServer.h>
#include "VehicleEvents.h"
#include "WebDashboardService.h"

// Must match the motor ESP32's WiFi.softAP(ssid, password) exactly.
static const char *WIFI_SSID     = "ESP32ForMotors";
static const char *WIFI_PASSWORD = "ilikecars";

// Static IP for the BCM board on the motor ESP32's AP subnet.
static IPAddress BCM_IP(192, 168, 4, 22);
static IPAddress GATEWAY(192, 168, 4, 1);   // the motor ESP32's AP address
static IPAddress SUBNET(255, 255, 255, 0);

static WebServer server(80);
static bool serverStarted = false;
static unsigned long lastReconnectAttempt = 0;

static void handleRoot()
{
    String html = R"rawliteral(
<!DOCTYPE html><html>
<head>
  <title>BCM Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    body { font-family: sans-serif; text-align: center; background:#111; color:#eee; }
    h2 { margin-top: 30px; }
    .card {
      display: inline-block; margin: 12px; padding: 20px 30px;
      border-radius: 10px; background: #222; min-width: 140px;
      font-size: 20px;
    }
    .label { font-size: 14px; color: #999; margin-bottom: 6px; }
    .locked   { color: #4caf50; }
    .unlocked { color: #f44336; }
    .on       { color: #ffeb3b; }
    .off      { color: #eee; }
    a { color: #4caf50; }
  </style>
</head>
<body>
  <h2>BCM Dashboard</h2>
  <div class="card">
    <div class="label">Door</div>
    <div id="door">--</div>
  </div>
  <div class="card">
    <div class="label">Lights</div>
    <div id="lights">--</div>
  </div>
  <div class="card">
    <div class="label">Cabin Temp</div>
    <div id="temp">--</div>
  </div>
  <p><a href="http://192.168.4.1/">Go to Car Controls</a></p>

  <script>
    async function refresh() {
      try {
        const res = await fetch("/status");
        const data = await res.json();

        const door = document.getElementById("door");
        door.textContent = data.door;
        door.className = data.door === "LOCKED" ? "locked" : "unlocked";

        const lights = document.getElementById("lights");
        lights.textContent = data.lights;
        lights.className = data.lights === "ON" ? "on" : "off";

        document.getElementById("temp").textContent = data.temp + " F";
      } catch (e) {
        // Server briefly unreachable - just retry on the next tick
      }
    }
    refresh();
    setInterval(refresh, 1000);
  </script>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
}

static void handleStatus()
{
    String json = "{";
    json += "\"door\":\"";
    json += (doorLocked ? "LOCKED" : "UNLOCKED");
    json += "\",\"lights\":\"";
    json += (headlightsOn ? "ON" : "OFF");
    json += "\",\"temp\":";
    json += cabinTemp;
    json += "}";

    server.send(200, "application/json", json);
}

void WebDashboardService_Init()
{
    WiFi.mode(WIFI_STA);
    WiFi.config(BCM_IP, GATEWAY, SUBNET);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("[WebDashboard] Connecting to ");
    Serial.println(WIFI_SSID);

    server.on("/", handleRoot);
    server.on("/status", handleStatus);
    // Note: server.begin() is deferred to the task, once WiFi actually
    // connects for the first time — see WebDashboardService() below.
}

void WebDashboardService(void *parameter)
{
    while (true)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            if (!serverStarted)
            {
                server.begin();
                serverStarted = true;
                Serial.print("[WebDashboard] Connected. Dashboard at http://");
                Serial.println(WiFi.localIP());
            }

            server.handleClient();
        }
        else
        {
            serverStarted = false;

            // Retry the connection every 5 seconds instead of blocking.
            unsigned long now = millis();
            if (now - lastReconnectAttempt > 10000)
            {
                lastReconnectAttempt = now;
                Serial.println("[WebDashboard] WiFi not connected, retrying...");

                // Fully tear down any in-progress connection attempt first.
                // Without this, calling WiFi.begin() while the driver is
                // still mid-connect throws "sta is connecting, cannot set
                // config" and the retry silently does nothing.
                WiFi.disconnect(true);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}