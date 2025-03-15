#include <ESP8266WiFi.h>
#include "WiFiManager.h"
#include "MQTTManager.h"
#include "ButtonManager.h"
#include "WebServerManager.h"
#include "IRModule.h"



// Configuration
const char* ssid = "almothaber";
const char* password = "awdr1234";
const char* mqtt_server = "192.168.1.24";
const char* mqtt_user = "mqtt_user";
const char* mqtt_pass = "securepassword";
const char* device_id = "esp8266_device";

WiFiClient espClient;
WiFiManager wifiManager(ssid, password);
MQTTManager mqttManager(mqtt_server, mqtt_user, mqtt_pass, device_id, espClient);
ButtonManager buttonManager(device_id, mqttManager.getClient()); // Use getClient()

IRModule irModule(D3, D2); // Adjust pins as needed
WebServerManager webServerManager(buttonManager, irModule); // Add IRModule reference

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // Turn off initially

    wifiManager.connect();
    mqttManager.connect();
    webServerManager.begin();
    irModule.begin();
}

void loop() {
    // Handle MQTT reconnection
    if (!mqttManager.getClient().connected()) {
        mqttManager.connect();
    }
    mqttManager.getClient().loop();

    // Handle LED blinking
    if (MQTTManager::ledBlinkFlag) {
        MQTTManager::ledBlinkFlag = false;
        Serial.println("From Call Back");
        irModule.transmitStoredSignal();
    }

    // Handle IR module loop
    irModule.handleLoop();

    // Handle web server
    webServerManager.handleClient();
}