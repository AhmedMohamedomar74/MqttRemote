#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define kRecvPin D3
#define kTrenPin D2  // Renamed to avoid conflict
#define kCaptureBufferSize 1024
#define kTimeout 50

extern IRrecv irrecv;
extern decode_results results;
extern uint16_t *RecRawBuff;
extern IRsend irsend;  // Use a different name for the IRsend object
extern bool RecivedSignal;


void setupIR();
void processIRSignal();

#endif