#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <EEPROM.h>  // Include EEPROM library

class ButtonManager {
public:
    ButtonManager(const char* device_id, PubSubClient& client);
    void registerButton(const String& button_name, const String& payload_press);

private:
    const char* _device_id;
    PubSubClient& _client;
    int _button_counter = 1;
    void saveButtonCounter();  // Function to save the counter to EEPROM
    void loadButtonCounter();  // Function to load the counter from EEPROM
};

#endif