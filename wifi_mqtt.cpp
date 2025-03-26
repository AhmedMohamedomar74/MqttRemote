#include <stdint.h>
#include "wifi_mqtt.h"

const char* ssid = "almothaber";
const char* password = "awdr1234";
const char* mqtt_server = "192.168.1.29";
const int mqtt_port = 1883;
const char* mqtt_user = "MQTT_USER";
const char* mqtt_password = "securepassword";
uint16_t * RecBuffuint_16 = nullptr;
bool RecBufferUint16Flag = false;
int RecBufferUintLegnth = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void OnMessage(char* topic, byte* payload, unsigned int length);

void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void setupMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(OnMessage);
  client.setBufferSize(2048);

  if (client.connect("ESP8266IRReceiver", mqtt_user, mqtt_password)) {
    Serial.println("Connected to MQTT Broker");
    if (client.subscribe("IR/Execute/HOAS")) {
      Serial.println("Subscribed to IR/Execute/HOAS");
    } else {
      Serial.println("Subscription failed");
    }
  } else {
    Serial.println("MQTT connection failed");
  }
}

void maintainMQTTConnection() {
  // while (!client.connected()) {
  //   Serial.println("Attempting MQTT connection...");
  //   if (client.connect("ESP8266IRReceiver", mqtt_user, mqtt_password)) {
  //     Serial.println("Connected to MQTT Broker");
  //   } else {
  //     Serial.print("Failed, rc=");
  //     Serial.print(client.state());
  //     Serial.println(" Retrying in 5 seconds...");
  //     delay(5000);
  //   }
  // }
}

void OnMessage(char* topic, byte* payload, unsigned int length) {
  irrecv.disableIRIn();
  Serial.print("Received message on topic: ");
  Serial.println(topic);

  // Convert payload to a string
  String payloadStr;
  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }

  Serial.print("Payload: ");
  Serial.println(payloadStr);

  // Remove the square brackets from the payload string
  payloadStr.replace("[", "");
  payloadStr.replace("]", "");

  // Split the string by commas
  int lastIndex = 0;
  for (int i = 0; i < payloadStr.length(); i++) {
    if (payloadStr.charAt(i) == ',') {
      RecBufferUintLegnth++;
    }
  }
  RecBufferUintLegnth++; // Number of elements is one more than the number of commas

  // Create a uint16_t array to store the RecBuffuint_16
  RecBuffuint_16 = new uint16_t[RecBufferUintLegnth];

  // Extract RecBuffuint_16 and store them in the array
  int index = 0;
  lastIndex = 0;
  for (int i = 0; i < payloadStr.length(); i++) {
    if (payloadStr.charAt(i) == ',' || i == payloadStr.length() - 1) {
      String numStr = payloadStr.substring(lastIndex, i + 1);
      numStr.trim(); // Remove any whitespace
      RecBuffuint_16[index] = numStr.toInt(); // Convert to integer
      lastIndex = i + 1;
      index++;
    }
  }
  Serial.println("Extracted RecBuffuint_16:");
    for (int i = 0; i < RecBufferUintLegnth; i++) {
      Serial.print(RecBuffuint_16[i]);
      Serial.print(",");
    }
    Serial.print("legnth of RecBuffer = ");
    Serial.println(RecBufferUintLegnth);
    irsend.sendRaw(RecBuffuint_16, RecBufferUintLegnth, 36);
    delay(500);
    irrecv.enableIRIn();
    RecBufferUintLegnth = 0;
    delete[] RecBuffuint_16;
}

