#include <Arduino.h>
#include "wifi_mqtt.h"
#include "ir_receiver.h"
#include "button_registration.h"
#include "web_server.h"

void executeTask() {
    Serial.println("Executing operation for button: " + registeredButtonName);
    Serial.println("State updated to Done");

    if (RecRawBuff && (!RecBufferUint16Flag)) {
      registerButton(_device_id, registeredButtonName, RecRawBuff, results.rawlen - 1);
      delete[] RecRawBuff;
      RecRawBuff = nullptr;
    }
    buttonRegistered = false; // Reset flag    
    Serial.println("Execution completed.");
    executionState = "Task Completed"; // Now modifies the global variable
}


void setup() {
  Serial.begin(115200);
  setupWiFi();
  setupMQTT();
  maintainMQTTConnection();
  setupIR();
  setupWebServer();
  EEPROMStorage_begin();
  loadButtonCounter();  // Load the last saved button counter value
}

void loop() {
  handleWebRequests();
  maintainMQTTConnection();
  client.loop();
  processIRSignal();
  if (buttonRegistered && executionState == "Waiting For IR" && RecivedSignal) {
        // executeTask();
        RecivedSignal = false;
        // delay(500);
        buttonRegistered = false;
        executionState = "Task Completed";
    }
   if(testFlag) {
        testFlag = false;
        irsend.sendRaw(RecRawBuff, results.rawlen - 1, 36);
        Serial.print("Length of the arr = ");
        Serial.println(results.rawlen - 1);
        Serial.print("Recived Buffer in test = ");
        for (int i = 0; i < results.rawlen - 1; i++) {
          Serial.print(RecRawBuff[i]);
          Serial.print(" ,");
        }
        Serial.println();
        Serial.println("Test button pressed");
    }
    
    if(yesFlag) {
        yesFlag = false;
        // Handle yes action
        registerButton(_device_id, registeredButtonName, RecRawBuff,(results.rawlen - 1));
        delete[] RecRawBuff;
        RecRawBuff = nullptr;
        executionState = "Waiting";
        Serial.println("Yes button pressed");
    }
    
    if(noFlag) {
        noFlag = false;
        // Handle no action
        delete[] RecRawBuff;
        RecRawBuff = nullptr;
        executionState = "Waiting";
         // Delete from EEPROM
        EEPROM_Status status = deleteStringFromEEPROM(registeredButtonName);
        if(status == EEPROM_SUCCESS) {
            Serial.println("Deleted from EEPROM: " + registeredButtonName);
        } else {
            Serial.println("EEPROM deletion failed. Error code: " + String(status));
        }
        registeredButtonName = "";  // Clear the name
        Serial.println("No button pressed");
    } 
}