#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <ArduinoJson.h>

#define kRecvPin D3 // TSOP1838 OUT pin
#define kSendPin D2 // IR LED pin

IRrecv irrecv(kRecvPin);
IRsend irsend(kSendPin);
decode_results results;

// Global JSON document pointer
StaticJsonDocument<128>* jsonCommand = nullptr; // Increased size to handle multiple signals

// Dynamic array for storing signals
int32 protocol = UNKNOWN; // Default to UNKNOWN (-1)
uint32_t* rawDataList = nullptr;
size_t signalCount = 0;
size_t signalCapacity = 0;

unsigned long lastSignalTime = 0;
const unsigned long timeout = 500; // Increased timeout to 500ms to capture multiple signals

// State variable to control listening mode
bool isListeningMode = false;

// Function to start listening mode
void startListeningMode() {
  // Reset data before starting to listen
  resetData(jsonCommand);
  isListeningMode = true; // Enable listening mode
  irrecv.enableIRIn(); // Reinitialize the IR receiver
  Serial.println("Listening for IR signals...");
}

// Function to add a new raw data value
void addSignal(decode_type_t detectedProtocol, uint32_t rawData) {
  if (protocol == UNKNOWN) {
    protocol = static_cast<uint32_t>(detectedProtocol);  // Store protocol as number
  }

  if (signalCount >= signalCapacity) {
    // Increase capacity dynamically (double it or start with 2)
    size_t newCapacity = (signalCapacity == 0) ? 2 : signalCapacity * 2;
    uint32_t* newData = new uint32_t[newCapacity];

    // Copy old data to new array
    for (size_t i = 0; i < signalCount; i++) {
      newData[i] = rawDataList[i];
    }

    // Free old memory and update pointers
    delete[] rawDataList;
    rawDataList = newData;
    signalCapacity = newCapacity;
  }

  // Add new raw data value
  rawDataList[signalCount] = rawData;
  signalCount++;
}

// Function to create a JSON object and store it globally
void createJsonCommand(StaticJsonDocument<128>*& jsonCommand) {
  if (jsonCommand != nullptr) {
    delete jsonCommand; // Free previous JSON command if it exists
  }

  jsonCommand = new StaticJsonDocument<128>(); // Create new JSON document

  (*jsonCommand)["protocol"] = protocol; // Store protocol as a number

  // Create JSON array for rawdata
  JsonArray rawDataArray = (*jsonCommand).createNestedArray("rawdata");
  for (size_t i = 0; i < signalCount; i++) {
    rawDataArray.add(rawDataList[i]); // Store as uint32_t
  }
}

// Function to print the JSON command
void printJson(StaticJsonDocument<128>* jsonCommand) {
  if (jsonCommand != nullptr) {
    serializeJson(*jsonCommand, Serial);
    Serial.println();
  } else {
    Serial.println("No JSON command available.");
  }
}

// Function to reset data and free memory
void resetData(StaticJsonDocument<128>*& jsonCommand) {
  // Clear stored data
  delete[] rawDataList;
  rawDataList = nullptr;
  signalCount = 0;
  signalCapacity = 0;
  protocol = UNKNOWN; // Reset protocol to UNKNOWN

  // Free JSON command memory
  if (jsonCommand != nullptr) {
    delete jsonCommand;
    jsonCommand = nullptr;
  }
}

// Function to send IR signal using JSON command
void sendIRSignal(StaticJsonDocument<128>* jsonCommand) {
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

  irsend.begin();

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
          case SAMSUNG: irsend.sendSAMSUNG(rawDataArray[i], 32); break;
          case NEC: irsend.sendNEC(rawDataArray[i], 32); break;
          case SONY: irsend.sendSony(rawDataArray[i], 12); break;
          case PANASONIC: irsend.sendPanasonic(0x4004, rawDataArray[i]); break;
          case JVC: irsend.sendJVC(rawDataArray[i], 16, 1); break;
          case LG: irsend.sendLG(rawDataArray[i], 32); break;
          case MITSUBISHI: irsend.sendMitsubishi(rawDataArray[i], 32); break;
          case SHARP: irsend.sendSharpRaw(rawDataArray[i], 32); break;
          case COOLIX: irsend.sendCOOLIX(rawDataArray[i], 32); break;
          case DAIKIN: irsend.sendDaikin64(rawDataArray[i]); break;
          case MIDEA: irsend.sendMidea(rawDataArray[i]); break;
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
      irsend.sendToshibaAC(toshibaData, kToshibaACStateLength, kToshibaACMinRepeat);
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
      irsend.sendFujitsuAC(fujitsuData, kFujitsuAcStateLength, kFujitsuAcMinRepeat);
      break;
    }

    default:
      Serial.println("Unsupported protocol.");
      return;
  }

  Serial.println("All IR signals sent.");
}

void setup() {
  Serial.begin(9600);
  pinMode(kRecvPin, INPUT_PULLUP);
  irrecv.enableIRIn();
  irsend.begin();
  Serial.println("ESP8266 ready. Send 'L' to start listening for IR signals.");
}

void loop() {
  // Check for serial input
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'L') {
      startListeningMode(); // Call the function to start listening mode
    } else if (command == 'T') {
      if (jsonCommand != nullptr) {
        sendIRSignal(jsonCommand); // Send the stored IR signal
        resetData(jsonCommand); // Reset data after transmission
      } else {
        Serial.println("No JSON command available to send.");
      }
    }
  }

  // Listen for IR signals if in listening mode
  if (isListeningMode && irrecv.decode(&results)) {
    if (results.decode_type != UNKNOWN) {
      addSignal(results.decode_type, results.value); // Store signal
      lastSignalTime = millis(); // Update timestamp
      irrecv.resume(); // Resume receiving next IR signal
    }
  }

  // Create JSON command if timeout is reached and in listening mode
  if (isListeningMode && signalCount > 0 && (millis() - lastSignalTime > timeout)) {
    createJsonCommand(jsonCommand);
    Serial.println("IR signals detected and JSON command created:");
    printJson(jsonCommand);
    isListeningMode = false; // Disable listening mode after capturing signals
  }
}