#include "WebServerManager.h"
#include "WiFiManager.h"
#include "MQTTManager.h"

ESP8266WebServer WebServerManager::server(80);
bool WebServerManager::configReceived = false;
const char* WebServerManager::mqtt_user = "";
const char* WebServerManager::mqtt_password = "";

const char* html_form = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP8266 Configuration</title>
</head>
<body>
  <h1>ESP8266 Configuration</h1>
  <form action="/save" method="POST">
    <label for="ssid">WiFi SSID:</label><br>
    <input type="text" id="ssid" name="ssid"><br>
    <label for="password">WiFi Password:</label><br>
    <input type="password" id="password" name="password"><br>
    <label for="mqtt_server">MQTT Broker:</label><br>
    <input type="text" id="mqtt_server" name="mqtt_server"><br>
    <label for="mqtt_user">MQTT Username:</label><br>
    <input type="text" id="mqtt_user" name="mqtt_user"><br>
    <label for="mqtt_password">MQTT Password:</label><br>
    <input type="password" id="mqtt_password" name="mqtt_password"><br><br>
    <input type="submit" value="Save and Connect">
  </form>
</body>
</html>
)rawliteral";

void WebServerManager::begin() {
  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.begin();
}

void WebServerManager::handleClient() {
  server.handleClient();
}

bool WebServerManager::isConfigReceived() {
  return configReceived;
}

const char* WebServerManager::getMQTTUser() {
  return mqtt_user;
}

const char* WebServerManager::getMQTTPassword() {
  return mqtt_password;
}

void WebServerManager::handleRoot() {
  server.send(200, "text/html", html_form);
}

void WebServerManager::handleSave() {
  const char* ssid = server.arg("ssid").c_str();
  const char* password = server.arg("password").c_str();
  const char* mqtt_server = server.arg("mqtt_server").c_str();
  mqtt_user = server.arg("mqtt_user").c_str();
  mqtt_password = server.arg("mqtt_password").c_str();

  // Attempt to connect to Wi-Fi
  if (WiFiManager::connect(ssid, password)) {
    Serial.println("Wi-Fi connected successfully!");

    // Close the AP since Wi-Fi is connected
    WiFiManager::disconnectAP();
    Serial.println("Access Point closed.");

    // Initialize MQTT
    MQTTManager::begin(mqtt_server, mqtt_user, mqtt_password);

    // Attempt to connect to MQTT broker in a loop
    bool mqttConnected = false;
    while (!mqttConnected) {
      if (MQTTManager::reconnect()) {
        Serial.println("MQTT connected successfully!");
        mqttConnected = true;
        configReceived = true;
        server.send(200, "text/plain", "Configuration saved and connected!");
      } else {
        Serial.println("MQTT connection failed. Reopening AP for reconfiguration...");

        // Reopen AP to allow reconfiguration
        WiFiManager::enableAP();
        Serial.println("Access Point reopened.");

        // Wait for new configuration from the user
        while (true) {
          server.handleClient(); // Keep handling client requests
          if (server.client()) { // Check if a client is connected
            break; // Exit the loop when a new client connects
          }
          delay(100);
        }

        // Get new MQTT details from the user
        mqtt_server = server.arg("mqtt_server").c_str();
        mqtt_user = server.arg("mqtt_user").c_str();
        mqtt_password = server.arg("mqtt_password").c_str();

        // Reinitialize MQTT with new details
        MQTTManager::begin(mqtt_server, mqtt_user, mqtt_password);
      }
    }
  } else {
    Serial.println("Wi-Fi connection failed!");

    // Re-enable AP to allow reconfiguration
    WiFiManager::enableAP();
    server.send(200, "text/plain", "Failed to connect to Wi-Fi. Please reconfigure.");
  }
}