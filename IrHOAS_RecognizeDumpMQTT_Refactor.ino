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
        executeTask();
        RecivedSignal = false;
        // delay(500);
        if (executionState == "Task Completed") {
          // executionState = "Waiting";
          Serial.println("buttonRegistered and executionState = Waiting For IR");
        }
    }
   if(testFlag) {
        testFlag = false;
        // Call your test function
        Serial.println("Test button pressed");
    }
    
    if(yesFlag) {
        yesFlag = false;
        // Handle yes action
        Serial.println("Yes button pressed");
    }
    
    if(noFlag) {
        noFlag = false;
        // Handle no action
        Serial.println("No button pressed");
    }
  
}