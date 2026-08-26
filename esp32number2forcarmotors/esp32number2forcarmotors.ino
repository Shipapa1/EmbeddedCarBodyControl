// //https://robotlk.com/
// //https://www.youtube.com/@RobotLk

// #include <WiFi.h>
// #include <WebServer.h>

// // Motor pins
// const int in1 = 26;
// const int in2 = 25;
// const int in3 = 33;
// const int in4 = 32;

// // Wi-Fi credentials (Access Point mode)
// const char* ssid = "ESP32ForMotors";
// const char* password = "ilikecars";

// // Create web server on port 80
// WebServer server(80);

// // Setup function
// void setup() {
//   Serial.begin(115200);

//   // Set motor pins as output
//   pinMode(in1, OUTPUT);
//   pinMode(in2, OUTPUT);
//   pinMode(in3, OUTPUT);
//   pinMode(in4, OUTPUT);

//   // Start Wi-Fi access point
//   WiFi.softAP(ssid, password);
//   Serial.println("WiFi AP Started");
//   Serial.print("IP Address: ");
//   Serial.println(WiFi.softAPIP());

//   // Define HTTP routes
//   server.on("/", handleRoot);
//   server.on("/F", forward);
//   server.on("/B", backward);
//   server.on("/L", left);
//   server.on("/R", right);
//   server.on("/S", stopCar);

//   server.begin();
// }

// // Main loop
// void loop() {
//   server.handleClient();
// }

// // HTML interface with JavaScript-based press-and-hold buttons
// void handleRoot() {
//   String html = R"rawliteral(
//     <!DOCTYPE html><html>
//     <head>
//       <title>ESP32 Car Control</title>
//       <meta name="viewport" content="width=device-width, initial-scale=1.0">
//       <style>
//         body { text-align: center; font-family: sans-serif; }
//         button {
//           width: 100px; height: 50px;
//           font-size: 16px; margin: 10px;
//         }
//       </style>
//       <script>
//         function sendCommand(cmd) {
//           fetch("/" + cmd);
//         }

//         function setupButton(id, command) {
//           const btn = document.getElementById(id);
//           btn.addEventListener('mousedown', () => sendCommand(command));
//           btn.addEventListener('mouseup', () => sendCommand('S'));
//           btn.addEventListener('touchstart', () => sendCommand(command));
//           btn.addEventListener('touchend', () => sendCommand('S'));
//         }

//         window.onload = () => {
//           setupButton("forward", "F");
//           setupButton("backward", "B");
//           setupButton("left", "L");
//           setupButton("right", "R");
//         };
//       </script>
//     </head>
//     <body>
//       <h2>ESP32 Web Controlled Car</h2>
//       <div>
//         <button id="forward">Forward</button><br>
//         <button id="left">Left</button>
//         <button id="right">Right</button><br>
//         <button id="backward">Backward</button>
//       </div>
//     </body>
//     </html>
//   )rawliteral";

//   server.send(200, "text/html", html);
// }

// // Movement control functions
// void forward() {
//   digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
//   digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
//   server.send(200, "text/plain", "Forward");
// }

// void backward() {
//   digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
//   digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
//   server.send(200, "text/plain", "Backward");
// }

// void left() {
//   digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
//   digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
//   server.send(200, "text/plain", "Left");
// }

// void right() {
//   digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
//   digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
//   server.send(200, "text/plain", "Right");
// }

// void stopCar() {
//   digitalWrite(in1, LOW); digitalWrite(in2, LOW);
//   digitalWrite(in3, LOW); digitalWrite(in4, LOW);
//   server.send(200, "text/plain", "Stop");
// }

//https://robotlk.com/
//https://www.youtube.com/@RobotLk

//https://robotlk.com/
//https://www.youtube.com/@RobotLk

#include <WiFi.h>
#include <WebServer.h>

// Motor pins
const int in1 = 26;
const int in2 = 25;
const int in3 = 33;
const int in4 = 32;

// Wi-Fi credentials (Access Point mode)
const char* ssid = "ESP32ForMotors";
const char* password = "ilikecars";

// Create web server on port 80
WebServer server(80);

// Forward declarations — needed because Arduino's auto-prototype generation
// can fail to pick these up on newer ESP32 core versions, in which case
// bare names like "left" and "right" resolve to std::left / std::right
// (iostream manipulators) instead of the functions defined below.
void handleRoot();
void forward();
void backward();
void left();
void right();
void stopCar();

// Setup function
void setup() {
  Serial.begin(115200);

  // Set motor pins as output
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // Start Wi-Fi access point
  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Define HTTP routes
  server.on("/", handleRoot);
  server.on("/F", forward);
  server.on("/B", backward);
  server.on("/L", left);
  server.on("/R", right);
  server.on("/S", stopCar);

  server.begin();
}

// Main loop
void loop() {
  server.handleClient();
}

// HTML interface with JavaScript-based press-and-hold buttons
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html><html>
    <head>
      <title>ESP32 Car Control</title>
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <style>
        body { text-align: center; font-family: sans-serif; }
        button {
          width: 100px; height: 50px;
          font-size: 16px; margin: 10px;
        }
      </style>
      <script>
        function sendCommand(cmd) {
          fetch("/" + cmd);
        }

        function setupButton(id, command) {
          const btn = document.getElementById(id);
          btn.addEventListener('mousedown', () => sendCommand(command));
          btn.addEventListener('mouseup', () => sendCommand('S'));
          btn.addEventListener('touchstart', () => sendCommand(command));
          btn.addEventListener('touchend', () => sendCommand('S'));
        }

        window.onload = () => {
          setupButton("forward", "F");
          setupButton("backward", "B");
          setupButton("left", "L");
          setupButton("right", "R");
        };
      </script>
    </head>
    <body>
      <h2>ESP32 Web Controlled Car</h2>
      <div>
        <button id="forward">Forward</button><br>
        <button id="left">Left</button>
        <button id="right">Right</button><br>
        <button id="backward">Backward</button>
      </div>
      <div style="margin-top: 20px;">
        <button onclick="window.location.href='http://192.168.4.22/'"
                style="width: auto; padding: 0 20px; background:#4caf50; color:white; border:none; border-radius:6px;">
          BCM Dashboard
        </button>
      </div>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// Movement control functions
void forward() {
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  server.send(200, "text/plain", "Forward");
}

void backward() {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  server.send(200, "text/plain", "Backward");
}

void left() {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  server.send(200, "text/plain", "Left");
}

void right() {
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  server.send(200, "text/plain", "Right");
}

void stopCar() {
  digitalWrite(in1, LOW); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW); digitalWrite(in4, LOW);
  server.send(200, "text/plain", "Stop");
}