#include <Wire.h>
#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h"
#include <PubSubClient.h>
#include <WiFi.h>  // Modify based on your connection method

// Replace with your Wi-Fi credentials
const char* ssid = "*******";
const char* password = "******";

// MQTT Broker settings
const char* mqttServer = "*********";
const int mqttPort = 1883;  // Default MQTT port
const char* mqttTopic = "Force";  // Change to your desired topic
const char* mqttUsername = "*******";  // Set your MQTT username
const char* mqttPassword = "*******";  // Set your MQTT password

WiFiClient espClient;
PubSubClient client(espClient);

NAU7802 myScale;
long zeroOffset = 0;
float calibrationFactor = 0.00034;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (myScale.begin() == false) {
    Serial.println("Scale not detected. Please check wiring. Freezing...");
    while (1);
  }
  Serial.println("Scale detected!");

  calibrateZero();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Generate a dynamic client ID
  String clientId = "ArduinoClient-" + String(millis());
 
  // Connect to MQTT broker with credentials and dynamic client ID
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    if (client.connect(clientId.c_str(), mqttUsername, mqttPassword)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {
  if (myScale.available()) {
    long currentReading = myScale.getReading() - zeroOffset;
    float force = currentReading * calibrationFactor;

    if (client.connected()) {
      char payload[20];
      snprintf(payload, sizeof(payload), "%.3f", force);
      client.publish(mqttTopic, payload);
    } else {
      Serial.println("MQTT not connected");
    }
  }
}

void calibrateZero() {
  // Calibration function

  long sum = 0;
  int samples = 100;

  for (int i = 0; i < samples; ++i) {
    sum += myScale.getReading();
    delay(10);
  }

  zeroOffset = sum / samples;
}
