#ifndef EEPROM_STRING_STORAGE_H
#define EEPROM_STRING_STORAGE_H

#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_SIZE 512  // EEPROM size
#define START_ADDRESS 4  // Start storing from byte 4

// Enum to indicate the status of EEPROM storage operation
typedef enum {
    EEPROM_SUCCESS,       // Successfully stored the string
    EEPROM_DUPLICATE,     // String is already stored
    EEPROM_NO_SPACE,      // Not enough space in EEPROM
    EEPROM_ERROR ,         // General error
    EEPROM_NOT_FOUND
} EEPROM_Status;

void EEPROMStorage_begin();
bool isStringStored(const String &str);
EEPROM_Status storeStringIfNotExists(const String &str);
EEPROM_Status deleteStringFromEEPROM(const String &str);

#endif // EEPROM_STRING_STORAGE_H
