#include <stdint.h>
#include "MQTTManager.h"
#include "WebServerManager.h"

WiFiClient MQTTManager::espClient;
PubSubClient MQTTManager::client(MQTTManager::espClient);
MqttTopicStatus Mqtt_topic_status = Not_Recieved;
StaticJsonDocument<128> jsonMQTTCommand;

void MQTTManager::begin(const char* server, const char* user, const char* password) {
  client.setServer(server, 1883);
  client.setCallback(onMessageReceived);  // Attach the callback function
}

bool MQTTManager::reconnect() {
  if (client.connect("ESP8266Client", WebServerManager::getMQTTUser(), WebServerManager::getMQTTPassword())) {
    Serial.println("Connected to MQTT broker!");
    client.subscribe("IR/Learn/HOAS");
    client.subscribe("IR/Learn/ESP");
    client.subscribe("IR/Execute/HOAS");
    return true;
  } else {
    Serial.println("MQTT connection failed, retrying...");
    delay(500);
    return false;
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

// Callback function to handle incoming MQTT messages
void MQTTManager::onMessageReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Parse the payload into a JSON command
  jsonMQTTCommand = MQTTManager::parsePayloadToJson(payload, length);  // Call the static function

  if (jsonMQTTCommand.isNull()) {
    Serial.println("Invalid JSON payload.");
    return;
  }

  // Handle the topic and JSON command
  if (strcmp(topic, "IR/Learn/HOAS") == 0) {
    Serial.println("Executing IR Learn HOAS sequence...");
    Mqtt_topic_status = Req;
  }
  //  else if (strcmp(topic, "IR/Learn/ESP") == 0) {
  //   Serial.println("Executing IR Learn ESP sequence...");
  //   Mqtt_topic_status = Learn;
  // } 
  else if (strcmp(topic, "IR/Execute/HOAS") == 0) {
    Serial.println("Executing IR Execute HOAS sequence...");
    Mqtt_topic_status = Execute;
  } else {
    Serial.println("Unknown topic received.");
  }

  // Print the JSON command for debugging
  Serial.println("Parsed JSON Command:");
  serializeJson(jsonMQTTCommand, Serial);
  Serial.println();

  char statusBuffer[10]; // Buffer to hold the string representation
  itoa(Mqtt_topic_status, statusBuffer, 10); // Convert integer to string (base 10)

  // Publish the status
  MQTTManager::publish("Test/MQTT", statusBuffer);
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
    return StaticJsonDocument<128>();  // Return an empty JSON document if parsing fails
  }

  return jsonDoc;
}