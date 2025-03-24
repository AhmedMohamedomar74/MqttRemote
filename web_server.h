#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "EEPROMStringStorage.h"
#include "wifi_mqtt.h"
extern bool buttonRegistered;
extern bool executionInProgress;
extern String registeredButtonName;
extern String executionState;

void setupWebServer();
void handleWebRequests();

#endif
