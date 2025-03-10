#include "IRModule.h"
#include <Arduino.h>

IRModule::IRModule(uint8_t recvPin, uint8_t sendPin) :
    irrecv(new IRrecv(recvPin)),
    irsend(new IRsend(sendPin)),
    jsonCommand(nullptr),
    rawDataList(nullptr),
    signalCount(0),
    signalCapacity(0),
    protocol(UNKNOWN),
    lastSignalTime(0),
    isListeningMode(false) {}

IRModule::~IRModule() {
    resetData();
    delete irrecv;
    delete irsend;
}

void IRModule::begin() {
    irrecv->enableIRIn();
    irsend->begin();
    Serial.println("IR Module Ready");
}

void IRModule::handleLoop() {
    decode_results results;
    if (isListeningMode && irrecv->decode(&results)) {
        if (results.decode_type != UNKNOWN) {
            addSignal(results.decode_type, results.value);
            lastSignalTime = millis();
        }
        irrecv->resume();
    }

    if (isListeningMode && signalCount > 0 && (millis() - lastSignalTime > timeout)) {
        createJsonCommand();
        Serial.println("IR command created:");
        printJson();
        isListeningMode = false;
    }
}

void IRModule::startListening() {
    resetData();
    isListeningMode = true;
    irrecv->enableIRIn();
    Serial.println("Listening for IR signals...");
}

void IRModule::transmitStoredSignal() {
    if (jsonCommand != nullptr) {
        sendIRSignal();
        resetData();
    } else {
        Serial.println("No command to send.");
    }
}

// Private methods implementation
void IRModule::addSignal(decode_type_t detectedProtocol, uint32_t rawData) {
    if (protocol == UNKNOWN && detectedProtocol != UNKNOWN) {
        protocol = detectedProtocol;
    }

    if (signalCount >= signalCapacity) {
        size_t newCapacity = (signalCapacity == 0) ? 2 : signalCapacity * 2;
        uint32_t* newData = new uint32_t[newCapacity];

        if (rawDataList) {
            memcpy(newData, rawDataList, signalCount * sizeof(uint32_t));
            delete[] rawDataList;
        }
        rawDataList = newData;
        signalCapacity = newCapacity;
    }

    rawDataList[signalCount++] = rawData;
}

void IRModule::createJsonCommand() {
    if (jsonCommand) delete jsonCommand;
    jsonCommand = new StaticJsonDocument<128>();

    if (protocol != UNKNOWN) {
        (*jsonCommand)["protocol"] = static_cast<uint8_t>(protocol);
    } else {
        (*jsonCommand)["protocol"] = nullptr;
    }

    JsonArray rawDataArray = (*jsonCommand).createNestedArray("rawdata");
    for (size_t i = 0; i < signalCount; i++) {
        rawDataArray.add(rawDataList[i]);
    }
}

void IRModule::printJson() {
    if (jsonCommand) {
        serializeJson(*jsonCommand, Serial);
        Serial.println();
    }
}

void IRModule::resetData() {
    delete[] rawDataList;
    rawDataList = nullptr;
    signalCount = 0;
    signalCapacity = 0;
    protocol = UNKNOWN;

    if (jsonCommand) {
        delete jsonCommand;
        jsonCommand = nullptr;
    }
}

void IRModule::sendIRSignal() {
    if (jsonCommand == nullptr) {
    Serial.println("No JSON command available to send.");
    return;
  }

  if (!jsonCommand->containsKey("protocol") || !jsonCommand->containsKey("rawdata")) {
    Serial.println("Invalid JSON: 'protocol' or 'rawdata' missing");
    return;
  }

  uint8_t protocol = (*jsonCommand)["protocol"];
  JsonArrayConst rawDataArray = (*jsonCommand)["rawdata"].as<JsonArrayConst>();
  size_t rawDataLength = rawDataArray.size();

  if (rawDataLength == 0) {
    Serial.println("Error: Empty rawdata array.");
    return;
  }

  irsend->begin();

  const char* protocolName;
  switch (protocol) {
    case SAMSUNG: protocolName = "SAMSUNG"; break;
    case NEC: protocolName = "NEC"; break;
    case SONY: protocolName = "SONY"; break;
    case PANASONIC: protocolName = "PANASONIC"; break;
    case JVC: protocolName = "JVC"; break;
    case LG: protocolName = "LG"; break;
    case MITSUBISHI: protocolName = "MITSUBISHI"; break;
    case SHARP: protocolName = "SHARP"; break;
    case COOLIX: protocolName = "COOLIX"; break;
    case DAIKIN: protocolName = "DAIKIN"; break;
    case HITACHI_AC: protocolName = "HITACHI_AC"; break;
    case TOSHIBA_AC: protocolName = "TOSHIBA_AC"; break;
    case FUJITSU_AC: protocolName = "FUJITSU_AC"; break;
    case MIDEA: protocolName = "MIDEA"; break;
    default: protocolName = "UNKNOWN"; break;
  }

  Serial.print("Detected Protocol: ");
  Serial.println(protocolName);

  switch (protocol) {
    case SAMSUNG:
    case NEC:
    case SONY:
    case PANASONIC:
    case JVC:
    case LG:
    case MITSUBISHI:
    case SHARP:
    case COOLIX:
    case DAIKIN:
    case MIDEA:
      for (size_t i = 0; i < rawDataLength; i++) {
        Serial.print("Sending ");
        Serial.print(protocolName);
        Serial.print(" signal: ");
        Serial.println(rawDataArray[i].as<uint32_t>(), HEX);

        switch (protocol) {
          case SAMSUNG: irsend->sendSAMSUNG(rawDataArray[i], 32); break;
          case NEC: irsend->sendNEC(rawDataArray[i], 32); break;
          case SONY: irsend->sendSony(rawDataArray[i], 12); break;
          case PANASONIC: irsend->sendPanasonic(0x4004, rawDataArray[i]); break;
          case JVC: irsend->sendJVC(rawDataArray[i], 16, 1); break;
          case LG: irsend->sendLG(rawDataArray[i], 32); break;
          case MITSUBISHI: irsend->sendMitsubishi(rawDataArray[i], 32); break;
          case SHARP: irsend->sendSharpRaw(rawDataArray[i], 32); break;
          case COOLIX: irsend->sendCOOLIX(rawDataArray[i], 32); break;
          case DAIKIN: irsend->sendDaikin64(rawDataArray[i]); break;
          case MIDEA: irsend->sendMidea(rawDataArray[i]); break;
        }
        delay(50);
      }
      break;

    case TOSHIBA_AC: {
      if (rawDataLength != kToshibaACStateLength) {
        Serial.println("Error: Incorrect Toshiba AC data length.");
        return;
      }
      uint8_t toshibaData[kToshibaACStateLength];
      for (size_t i = 0; i < rawDataLength; i++) {
        toshibaData[i] = rawDataArray[i];
      }
      Serial.println("Sending Toshiba AC Signal...");
      irsend->sendToshibaAC(toshibaData, kToshibaACStateLength, kToshibaACMinRepeat);
      break;
    }

    case FUJITSU_AC: {
      if (rawDataLength != kFujitsuAcStateLength) {
        Serial.println("Error: Incorrect Fujitsu AC data length.");
        return;
      }
      unsigned char fujitsuData[kFujitsuAcStateLength];
      for (size_t i = 0; i < rawDataLength; i++) {
        fujitsuData[i] = rawDataArray[i];
      }
      Serial.println("Sending Fujitsu AC Signal...");
      irsend->sendFujitsuAC(fujitsuData, kFujitsuAcStateLength, kFujitsuAcMinRepeat);
      break;
    }

    default:
      Serial.println("Unsupported protocol.");
      return;
  }

  Serial.println("All IR signals sent.");
}