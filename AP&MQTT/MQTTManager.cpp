#include "MQTTManager.h"
#include "WebServerManager.h" // Include WebServerManager header

WiFiClient MQTTManager::espClient;
PubSubClient MQTTManager::client(MQTTManager::espClient);

void MQTTManager::begin(const char* server, const char* user, const char* password) {
  client.setServer(server, 1883);
}

bool MQTTManager::reconnect() {
  if (client.connect("ESP8266Client", WebServerManager::getMQTTUser(), WebServerManager::getMQTTPassword())) {
    Serial.println("Connected to MQTT broker!");
    return true;
  } else {
    Serial.println("MQTT connection failed, retrying...");
    delay(500);
    return false; // Ensure a value is returned
  }
}

void MQTTManager::loop() {
  client.loop();
}

bool MQTTManager::isConnected() {
  return client.connected();
}

void MQTTManager::publish(const char* topic, const char* message) {
  if (client.connected()) {
    if (client.publish(topic, message)) {
      Serial.println("Message published to topic: " + String(topic));
    } else {
      Serial.println("Failed to publish message!");
    }
  } else {
    Serial.println("MQTT client not connected!");
  }
}