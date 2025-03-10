#include "WiFiManager.h"
#include "MQTTManager.h"
#include "WebServerManager.h"

void setup() {
  Serial.begin(9600);

  // Initialize WiFiManager
  WiFiManager::begin();

  // Initialize WebServerManager
  WebServerManager::begin();

  Serial.println("AP started");
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}

void loop() {
  // Handle web server requests
  WebServerManager::handleClient();

  // Maintain MQTT connection if configuration is received
  if (WebServerManager::isConfigReceived() && !MQTTManager::isConnected()) {
    MQTTManager::reconnect();
  }
  if (MQTTManager::isConnected())
  {
    MQTTManager::publish("learn/IR","Hellow From ESP8266");
  }
  MQTTManager::loop();
}