#include <WiFi.h>
#include <PubSubClient.h>

#define PIR_PIN 23
#define RELAY_PIN 17
#define ssid "IoT"
#define password "@IoT@S3nh@S3gur@"
#define mqtt_server "192.168.15.10"
#define MQTT_PORT 1883
#define MQTT_USER "RobsonBrasil"
#define MQTT_PASSWORD "loboalfa"
#define MQTT_STATE_TOPIC "SensorPIR/Estado"
#define MQTT_COMMAND_TOPIC "SensorPIR/Comando"

volatile bool motionDetected = false;

WiFiClient espClient;
PubSubClient client(espClient);

TaskHandle_t core1TaskHandle = NULL;

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

    // Set up MQTT client and connect to broker
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Connect to MQTT broker
  //client.setServer(mqtt_server, MQTT_PORT);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("ESP32Client", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(MQTT_COMMAND_TOPIC);
    } else {
      Serial.print("Failed to connect to MQTT broker with state ");
      Serial.println(client.state());
      delay(1000);
    }
  }
  
  // Create a task to run on core 1
  xTaskCreatePinnedToCore(task_core1, "core1", 10000, NULL, 1, &core1TaskHandle, 1);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publish the current state of the PIR sensor to the MQTT broker
  String state = motionDetected ? "ON" : "OFF";
  client.publish(MQTT_COMMAND_TOPIC, state.c_str());

  delay(1000);
}

void task_core1(void *pvParameters) {
  // Loop to read PIR sensor and control relay
  while (1) {
    int pirValue = digitalRead(PIR_PIN);

    // If PIR sensor detects motion, turn on relay and publish to MQTT
    if (pirValue == HIGH) {
      digitalWrite(RELAY_PIN, LOW);
      client.publish("state", "ON");
    } else {
      digitalWrite(RELAY_PIN, HIGH);
      client.publish("state", "OFF");
    }

    // Wait a short amount of time before reading PIR sensor again
    delay(100);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Reconnecting to MQTT broker...");
    if (client.connect("ESP32Client", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(MQTT_STATE_TOPIC);
    } else {
      Serial.print("Failed to connect to MQTT broker with state ");
      Serial.println(client.state());
      delay(1000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Print topic and message payload
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Handle incoming messages here
}