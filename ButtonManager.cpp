#include "ButtonManager.h"

ButtonManager::ButtonManager(const char* device_id, PubSubClient& client)
    : _device_id(device_id), _client(client) {
    EEPROM.begin(512);  // Initialize EEPROM with 512 bytes
    loadButtonCounter();  // Load the counter from EEPROM
}

void ButtonManager::registerButton(const String& button_name, const String& payload_press) {
    String button_id = "button_" + String(_button_counter);
    String unique_id = String(_device_id) + "_" + button_id;
    String command_topic = "IR/Execute/HOAS";

    StaticJsonDocument<512> doc;
    doc["name"] = button_name;
    doc["command_topic"] = command_topic;
    doc["payload_press"] = payload_press;
    doc["unique_id"] = unique_id;
    JsonObject device = doc.createNestedObject("device");
    device["identifiers"] = _device_id;
    device["name"] = "IrRemote";
    device["manufacturer"] = "ESP8266";

    String discovery_topic = "homeassistant/button/" + String(_device_id) + "/" + button_id + "/config";
    String payload;
    serializeJson(doc, payload);

    Serial.print("Publishing to topic: ");
    Serial.println(discovery_topic);
    Serial.print("Payload: ");
    Serial.println(payload);

    delay(100); // Small delay to ensure MQTT stability
    _client.publish(discovery_topic.c_str(), payload.c_str(), false);
    _button_counter++;
    saveButtonCounter();  // Save the updated counter to EEPROM
}

void ButtonManager::saveButtonCounter() {
    EEPROM.put(0, _button_counter);  // Save the counter at address 0
    EEPROM.commit();  // Commit the changes to EEPROM
}

void ButtonManager::loadButtonCounter() {
    EEPROM.get(0, _button_counter);  // Load the counter from address 0
    if (_button_counter < 1) {
        _button_counter = 1;  // Ensure the counter is at least 1
    }
}