#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include <ESP8266WebServer.h>

class WebServerManager {
public:
  static void begin();
  static void handleClient();
  static bool isConfigReceived();
  static const char* getMQTTUser(); // Declare as static
  static const char* getMQTTPassword(); // Declare as static

private:
  static void handleRoot();
  static void handleSave();
  static ESP8266WebServer server;
  static bool configReceived;
  static const char* mqtt_user;
  static const char* mqtt_password;
};

#endif