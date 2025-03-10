#include "IRModule.h"

#define RECV_PIN D3
#define SEND_PIN D2

IRModule irModule(RECV_PIN, SEND_PIN);

void setup() {
    Serial.begin(9600);
    irModule.begin();
    Serial.println("System Ready. Send L/T commands");
}

void loop() {
    irModule.handleLoop();
    
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        if (cmd == 'L') {
            irModule.startListening();
        } else if (cmd == 'T') {
            irModule.transmitStoredSignal();
        }
    }
}