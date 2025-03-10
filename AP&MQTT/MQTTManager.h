#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <PubSubClient.h>
#include <ESP8266WiFi.h>

class MQTTManager {
public:
  static void begin(const char* server, const char* user, const char* password);
  static bool reconnect();
  static void loop();
  static bool isConnected();
  static void publish(const char* topic, const char* message);

private:
  static WiFiClient espClient;
  static PubSubClient client;
};

#endif