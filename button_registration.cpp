#include "button_registration.h"

int _button_counter = 1;
const char* _device_id = "esp8266_device";

void registerButton(const char* _device_id, const String& button_name, uint16_t* payload_press, size_t payload_size) {
    loadButtonCounter();  // Load the counter from EEPROM

    String button_id = "button_" + String(_button_counter);
    String unique_id = String(_device_id) + "_" + button_id;
    String command_topic = "IR/Execute/HOAS";
    String discovery_topic = "homeassistant/button/" + String(_device_id) + "/" + button_id + "/config";
    Serial.println(discovery_topic.c_str());

    StaticJsonDocument<512> doc;
    doc["name"] = button_name;
    doc["command_topic"] = command_topic;

    String payloadPressStr;
    payloadPressStr.reserve(50);
    payloadPressStr = "[";
    for (size_t i = 0; i < payload_size; i++) {
        payloadPressStr += String(payload_press[i]);
        if (i < payload_size - 1) {
            payloadPressStr += ",";
        }
    }
    payloadPressStr += "]";

    doc["payload_press"] = payloadPressStr;
    doc["unique_id"] = unique_id;

    JsonObject device = doc.createNestedObject("device");
    device["identifiers"] = _device_id;
    device["name"] = "IrRemote";
    device["manufacturer"] = "ESP8266";

    String payload;
    serializeJson(doc, payload);

    Serial.print("Payload: ");
    Serial.println(payload);

    if (client.publish(discovery_topic.c_str(), payload.c_str(), false)) {
        Serial.println("Payload published to Home Assistant");

        _button_counter++;  // Increment after successful publish
        saveButtonCounter();  // Save new counter value

        doc.clear();
        payloadPressStr = "";
        payload = "";
    } else {
        Serial.println("Publish failed!");
    }
}

void saveButtonCounter() {
    EEPROM.put(4, _button_counter);  // Store the counter at address 4
    EEPROM.commit();  // Commit the changes
}


void loadButtonCounter() {
    EEPROM.begin(4);  // Initialize EEPROM
    
    int magic_number;
    EEPROM.get(0, magic_number);  // Read stored magic number

    if (magic_number != 0xA5A5A5A5) {  // Check if EEPROM is uninitialized
        Serial.println("First boot detected! Resetting counter...");
        _button_counter = 1;  // Start from 1
        EEPROM.put(0, 0xA5A5A5A5);  // Store magic number
        EEPROM.put(4, _button_counter);  // Save initial counter
        EEPROM.commit();  // Commit changes
    } else {
        EEPROM.get(4, _button_counter);  // Load existing counter
        if (_button_counter < 1) {
            _button_counter = 1;  // Ensure valid range
        }
    }
}

