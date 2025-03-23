#include "ir_receiver.h"

IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
IRsend irsend(kTrenPin);  // Use the renamed pin definition
decode_results results;
uint16_t *RecRawBuff = nullptr;
bool RecivedSignal = false;

void setupIR() {
  irrecv.enableIRIn();
  irsend.begin();
  Serial.println("IR Receiver Ready!");
}

void processIRSignal() {
  if (irrecv.decode(&results) && (executionState == "Waiting For IR")) {
    if (RecRawBuff) {
      delete[] RecRawBuff;
    }

    RecRawBuff = new uint16_t[results.rawlen - 1];

    for (uint16_t i = 1; i < results.rawlen; i++) {
      RecRawBuff[i - 1] = results.rawbuf[i] * kRawTick;
    }
    RecivedSignal = true;
    irrecv.resume();
  }
}