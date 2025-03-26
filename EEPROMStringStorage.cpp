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

EEPROM_Status deleteStringFromEEPROM(const String &str) {
    int startIdx = -1;
    int endIdx = -1;
    bool found = false;

    // Search for the string in EEPROM
    for (int i = START_ADDRESS; i < EEPROM_SIZE; i++) {
        String storedStr = "";
        int j = i;

        // Read characters until we find '\0' (end of string)
        while (j < EEPROM_SIZE) {
            char c = EEPROM.read(j);
            if (c == '\0' || c == 0xFF) break;
            storedStr += c;
            j++;
        }

        if (storedStr == str) {
            startIdx = i;
            endIdx = j;  // Position of null terminator
            found = true;
            break;
        }

        i = j;  // Move to the next string
    }

    if (!found) {
        return EEPROM_NOT_FOUND;
    }

    int remainingBytes = EEPROM_SIZE - endIdx - 1;

    // Shift remaining data forward
    for (int i = startIdx; i < startIdx + remainingBytes; i++) {
        char nextChar = EEPROM.read(endIdx + (i - startIdx));
        EEPROM.write(i, nextChar);
    }

    // Fill the remaining space with 0xFF (erase)
    for (int i = EEPROM_SIZE - (endIdx - startIdx); i < EEPROM_SIZE; i++) {
        EEPROM.write(i, 0xFF);
    }

    EEPROM.commit();
    return EEPROM_SUCCESS;
}

