#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

typedef enum
{
  Not_Recieved = 0,
  Req,
  Learn,
  Execute
} MqttTopicStatus;

extern StaticJsonDocument<128> jsonMQTTCommand;

class MQTTManager {
public:
  static void begin(const char* server, const char* user, const char* password);
  static bool reconnect();
  static void loop();
  static bool isConnected();
  static void publish(const char* topic, const char* message);

  // Declare the function as static
  static StaticJsonDocument<128> parsePayloadToJson(const byte* payload, unsigned int length);

private:
  static WiFiClient espClient;
  static PubSubClient client;
  static void onMessageReceived(char* topic, byte* payload, unsigned int length);
};

extern MqttTopicStatus Mqtt_topic_status;

#endif