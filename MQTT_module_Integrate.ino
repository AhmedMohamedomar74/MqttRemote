#include "WiFiManager.h"
#include "MQTTManager.h"
#include "WebServerManager.h"
#include "IRModule.h"

#define RECV_PIN D3
#define SEND_PIN D2

IRModule irModule(RECV_PIN, SEND_PIN);

void setup() {
  Serial.begin(9600);

  // Initialize WiFiManager
  WiFiManager::begin();

  // Initialize WebServerManager
  WebServerManager::begin();

  irModule.begin();

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
  MQTTManager::loop();
  irModule.handleLoop();
  if (Mqtt_topic_status == Req) {
    irModule.startListening();
    Mqtt_topic_status = Not_Recieved;
  }
  else if (Mqtt_topic_status == Execute) {
    irModule.transmitStoredSignal();
    Mqtt_topic_status = Not_Recieved;
  }
}