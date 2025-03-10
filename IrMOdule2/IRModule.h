#ifndef IR_MODULE_H
#define IR_MODULE_H

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <ArduinoJson.h>

class IRModule {
public:
    IRModule(uint8_t recvPin, uint8_t sendPin);
    ~IRModule();

    void begin();
    void handleLoop();
    void startListening();
    void transmitStoredSignal();

private:
    IRrecv* irrecv;
    IRsend* irsend;
    
    StaticJsonDocument<128>* jsonCommand;
    uint32_t* rawDataList;
    size_t signalCount;
    size_t signalCapacity;
    decode_type_t protocol;
    unsigned long lastSignalTime;
    bool isListeningMode;
    
    void addSignal(decode_type_t detectedProtocol, uint32_t rawData);
    void createJsonCommand();
    void printJson();
    void resetData();
    void sendIRSignal();
    
    static const unsigned long timeout = 500;
};

#endif