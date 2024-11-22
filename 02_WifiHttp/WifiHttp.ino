#include <WiFi.h>
#include <WebServer.h>

// Open Wi-Fi network settings
const char* ssid = "ESP32_Hotspot";  // Wi-Fi network name (open)
const char* password = "";           // No password (open network)

// Hardcoded credentials for login
const char* correctUsername = "admin";
const char* correctPassword = "password123";

// Web server setup on port 80
WebServer server(80);

// GPIO pin for controlling the LED
const int ledPin = 2;

// To store connected devices (MAC addresses)
String connectedDevices = "";

// LED State variable
bool ledState = false;

void setup() {
  Serial.begin(115200);

  // Set up LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Ensure LED is initially off

  // Set up ESP32 as an open access point (no password)
  WiFi.softAP(ssid, password);
  Serial.println("ESP32 Hotspot created.");
  Serial.print("Access Point IP: ");
  Serial.println(WiFi.softAPIP());

  // Handle root URL (login form)
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", 
    "<html><head>"
    "<style>"
    "body { font-family: 'Arial', sans-serif; margin: 0; padding: 0; background: #212121; color: #fff; text-align: center; }"
    "h1 { font-size: 36px; font-weight: bold; margin-top: 50px; color: #e0a300; }"
    "input[type='text'], input[type='password'] {"
    "  padding: 12px; font-size: 16px; margin: 10px; border-radius: 5px; border: none; width: 80%; max-width: 400px;"
    "  background: #333; color: #fff; box-sizing: border-box; }"
    "input[type='submit'] {"
    "  padding: 12px 30px; background: #e0a300; color: white; border: none; border-radius: 5px; font-size: 18px;"
    "  cursor: pointer; }"
    "input[type='submit']:hover { background: #c68200; }"
    "a { color: #e0a300; text-decoration: none; font-size: 18px; }"
    "a:hover { text-decoration: underline; }"
    ".infotainment {"
    "  background: #333; color: #fff; border-radius: 5px; padding: 20px; margin-top: 30px; width: 80%; max-width: 600px; margin-left: auto; margin-right: auto;"
    "  text-align: left; font-size: 18px; }"
    "</style>"
    "</head><body>"
    "<h1>Mercedes-Benz ESP32 Web Portal</h1>"
    "<form action='/login' method='POST'>"
    "<input type='text' name='username' placeholder='Username'><br>"
    "<input type='password' name='password' placeholder='Password'><br>"
    "<input type='submit' value='Login'>"
    "</form></body></html>");
  });

  // Handle login logic
  server.on("/login", HTTP_POST, []() {
    String username = server.arg("username");
    String password = server.arg("password");

    // Validate credentials
    if (username == correctUsername && password == correctPassword) {
      server.send(200, "text/html", "<html><head><style>"
      "body { font-family: 'Arial', sans-serif; background: #212121; color: #fff; text-align: center; }"
      "h1 { font-size: 36px; color: #e0a300; margin-top: 50px; }"
      "input[type='submit'] { background: #e0a300; color: white; border: none; padding: 12px 30px; font-size: 18px; border-radius: 5px; cursor: pointer; }"
      "input[type='submit']:hover { background: #c68200; }"
      ".infotainment {"
      "  background: #333; color: #fff; border-radius: 5px; padding: 20px; margin-top: 30px; width: 80%; max-width: 600px; margin-left: auto; margin-right: auto;"
      "  text-align: left; font-size: 18px; }"
      "</style></head><body>"
      "<h1>Login Successful</h1>"
      "<form action='/control' method='GET'><input type='submit' value='Turn On LED'></form><br><br>"
      "<form action='/control_off' method='GET'><input type='submit' value='Turn Off LED'></form><br><br>"
      "<div class='infotainment'><h2>Infotainment Panel</h2>"
      "<p><strong>LED State:</strong> " + String(ledState ? "ON" : "OFF") + "</p>"
      "<p><strong>Connected Devices:</strong> " + connectedDevices + "</p>"
      "</div></body></html>");

      // Log the attacker's MAC address (or any device that connects)
      connectedDevices += WiFi.softAPmacAddress() + "<br>";
    } else {
      server.send(401, "text/html", "<html><head><style>"
      "body { font-family: 'Arial', sans-serif; background: #212121; color: #fff; text-align: center; }"
      "h1 { font-size: 36px; color: #e0a300; margin-top: 50px; }"
      "input[type='submit'] { background: #e0a300; color: white; border: none; padding: 12px 30px; font-size: 18px; border-radius: 5px; cursor: pointer; }"
      "input[type='submit']:hover { background: #c68200; }"
      "</style></head><body>"
      "<h1>Login Failed</h1><a href='/'>Try Again</a>"
      "</body></html>");
    }
  });

  // Handle GPIO control for turning LED on
  server.on("/control", HTTP_GET, []() {
    digitalWrite(ledPin, HIGH);  // Turn on LED
    ledState = true;
    server.send(200, "text/html", "<html><head><style>"
    "body { font-family: 'Arial', sans-serif; background: #212121; color: #fff; text-align: center; }"
    "h1 { font-size: 36px; color: #e0a300; margin-top: 50px; }"
    "input[type='submit'] { background: #e0a300; color: white; border: none; padding: 12px 30px; font-size: 18px; border-radius: 5px; cursor: pointer; }"
    "input[type='submit']:hover { background: #c68200; }"
    "</style></head><body>"
    "<h1>LED is ON</h1>"
    "<form action='/control' method='GET'><input type='submit' value='Turn On LED'></form><br><br>"
    "<form action='/control_off' method='GET'><input type='submit' value='Turn Off LED'></form><br><br>"
    "<div class='infotainment'><h2>Infotainment Panel</h2>"
    "<p><strong>LED State:</strong> ON</p>"
    "<p><strong>Connected Devices:</strong> " + connectedDevices + "</p>"
    "</div></body></html>");
  });

  // Handle GPIO control for turning LED off
  server.on("/control_off", HTTP_GET, []() {
    digitalWrite(ledPin, LOW);  // Turn off LED
    ledState = false;
    server.send(200, "text/html", "<html><head><style>"
    "body { font-family: 'Arial', sans-serif; background: #212121; color: #fff; text-align: center; }"
    "h1 { font-size: 36px; color: #e0a300; margin-top: 50px; }"
    "input[type='submit'] { background: #e0a300; color: white; border: none; padding: 12px 30px; font-size: 18px; border-radius: 5px; cursor: pointer; }"
    "input[type='submit']:hover { background: #c68200; }"
    "</style></head><body>"
    "<h1>LED is OFF</h1>"
    "<form action='/control' method='GET'><input type='submit' value='Turn On LED'></form><br><br>"
    "<form action='/control_off' method='GET'><input type='submit' value='Turn Off LED'></form><br><br>"
    "<div class='infotainment'><h2>Infotainment Panel</h2>"
    "<p><strong>LED State:</strong> OFF</p>"
    "<p><strong>Connected Devices:</strong> " + connectedDevices + "</p>"
    "</div></body></html>");
  });

  // Start the web server
  server.begin();
}

void loop() {
  server.handleClient();  // Handle incoming requests
}
