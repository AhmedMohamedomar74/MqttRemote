#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
extern StaticJsonDocument<128> jsonMQTTCommand;

class MQTTManager {
public:
    MQTTManager(const char* server, const char* user, const char* pass, const char* device_id, WiFiClient& client);
    void connect();
    void publish(const char* topic, const char* payload);
    PubSubClient& getClient();
    static StaticJsonDocument<128> parsePayloadToJson(const byte* payload, unsigned int length);
    // Static members for LED control
    static bool ledBlinkFlag;

private:
    const char* _server;
    const char* _user;
    const char* _pass;
    const char* _device_id;
    PubSubClient _client;
    static void onMessageReceived(char* topic, byte* payload, unsigned int length);
};

#endif