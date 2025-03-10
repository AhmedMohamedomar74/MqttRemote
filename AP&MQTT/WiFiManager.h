#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <ESP8266WiFi.h>

class WiFiManager {
public:
  static void begin();
  static bool connect(const char* ssid, const char* password);
  static void disconnectAP();
  static void enableAP();
  static bool isConnected();
};

#endif