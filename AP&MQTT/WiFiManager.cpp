#include "WiFiManager.h"

void WiFiManager::begin() {
  // Start the ESP8266 in AP mode
  WiFi.softAP("ESP8266_Config");
}

bool WiFiManager::connect(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected successfully!");
    Serial.println("IP address: " + WiFi.localIP().toString());
    return true;
  } else {
    Serial.println("\nFailed to connect to Wi-Fi!");
    return false;
  }
}

void WiFiManager::disconnectAP() {
  WiFi.softAPdisconnect(true);
  Serial.println("AP disconnected");
}

void WiFiManager::enableAP() {
  WiFi.softAP("ESP8266_Config");
  Serial.println("AP re-enabled for new configuration");
}

bool WiFiManager::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}