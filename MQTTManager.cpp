#include "MQTTManager.h"

// Initialize static members
bool MQTTManager::ledBlinkFlag = false;
StaticJsonDocument<128> jsonMQTTCommand;

MQTTManager::MQTTManager(const char* server, const char* user, const char* pass, const char* device_id, WiFiClient& client)
    : _server(server), _user(user), _pass(pass), _device_id(device_id), _client(client) {
    _client.setBufferSize(512);
    _client.setServer(_server, 1883);
    _client.setCallback(onMessageReceived); // Set callback
    _client.subscribe("IR/Execute/HOAS"); // Subscribe to the topic
}

void MQTTManager::connect() {
    while (!_client.connected()) {
        Serial.print("Connecting to MQTT...");
        if (_client.connect(_device_id, _user, _pass)) {
            Serial.println("Connected!");
            _client.subscribe("IR/Execute/HOAS"); // Re-subscribe after reconnect
        } else {
            Serial.print("Failed, rc=");
            Serial.print(_client.state());
            Serial.println(" retrying...");
            delay(2000);
        }
    }
}
void MQTTManager::publish(const char* topic, const char* payload) {
    if (_client.publish(topic, payload, false)) {
        Serial.println("Message published successfully.");
    } else {
        Serial.println("Failed to publish message.");
    }
}

PubSubClient& MQTTManager::getClient() {
    return _client;
}

// Implement the static function
StaticJsonDocument<128> MQTTManager::parsePayloadToJson(const byte* payload, unsigned int length) {
  StaticJsonDocument<128> jsonDoc;

  // Convert the payload to a string
  String payloadStr;
  for (unsigned int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }
  // Deserialize the JSON string into the JSON document
  DeserializationError error = deserializeJson(jsonDoc, payloadStr);

  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    return StaticJsonDocument<128>();  // Return empty doc on error
  }

  return jsonDoc;
}
// Callback to trigger LED blinking
void MQTTManager::onMessageReceived(char* topic, byte* payload, unsigned int length) {
    Serial.print("Received message on topic: ");
    Serial.println(topic);
    // Parse the payload into a JSON command
    jsonMQTTCommand = MQTTManager::parsePayloadToJson(payload,length);  // Call the static function
    // Blink LED if topic matches
    if (strcmp(topic, "IR/Execute/HOAS") == 0) {
        ledBlinkFlag = true;
    }
}