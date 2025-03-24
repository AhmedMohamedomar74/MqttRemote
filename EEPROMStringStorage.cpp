#include "EEPROMStringStorage.h"

void EEPROMStorage_begin() {
    EEPROM.begin(EEPROM_SIZE);
}

bool isStringStored(const String &str) {
    String storedStr = "";
    for (int i = START_ADDRESS; i < EEPROM_SIZE; i++) {
        char c = EEPROM.read(i);
        if (c == '\0' || c == 0xFF) break;  // Stop at null terminator or empty EEPROM
        storedStr += c;
    }
    return storedStr == str;
}

EEPROM_Status storeStringIfNotExists(const String &str) {
    if (isStringStored(str)) {
        return EEPROM_DUPLICATE;
    }

    if (START_ADDRESS + str.length() + 1 >= EEPROM_SIZE) {
        return EEPROM_NO_SPACE;
    }

    for (int i = 0; i < str.length(); i++) {
        EEPROM.write(START_ADDRESS + i, str[i]);
    }
    EEPROM.write(START_ADDRESS + str.length(), '\0');  // Add null terminator

    EEPROM.commit();
    return EEPROM_SUCCESS;
}
