#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "ir_receiver.h"

extern const char* ssid;
extern const char* password;
extern const char* mqtt_server;
extern const int mqtt_port;
extern const char* mqtt_user;
extern const char* mqtt_password;

extern WiFiClient espClient;
extern PubSubClient client;

extern uint16_t* RecBuffuint_16;
extern bool RecBufferUint16Flag;
extern int RecBufferUintLegnth;

void setupWiFi();
void setupMQTT();
void maintainMQTTConnection();

#endif