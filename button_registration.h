#ifndef BUTTON_REGISTRATION_H
#define BUTTON_REGISTRATION_H

#include <ArduinoJson.h>
#include "wifi_mqtt.h"
#include <EEPROM.h>  // Include EEPROM library

extern int _button_counter;
extern const char* _device_id;

void registerButton(const char* _device_id, const String& button_name, uint16_t* payload_press, size_t payload_size);
void loadButtonCounter();
void saveButtonCounter();

#endif