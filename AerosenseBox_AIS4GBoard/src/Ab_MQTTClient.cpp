#include "Ab_MQTTClient.h"

Ab_MQTTClient::Ab_MQTTClient(const char* id, const char* broker, int port, const char* username, const char* password)
    : mqttClient(gsmClient), id(id), broker(broker), port(port), username(username), password(password) {}

void Ab_MQTTClient::begin() {
    while (!GSM.begin()) {
        Serial.println("GSM setup failed");
        delay(100);
    }

    mqttClient.setServer(broker, port);
    mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->callback(topic, payload, length);
    });
    mqttClient.connect(id.c_str(), username, password);

}
bool Ab_MQTTClient::isConnected() {
  return mqttClient.connected();
}
void Ab_MQTTClient::loop() {
        mqttClient.loop();
    
}
void Ab_MQTTClient::publish(const char* topic, const char* payload) {
    mqttClient.publish(topic, payload);
}

void Ab_MQTTClient::subscribe(const char* topic) {
    mqttClient.subscribe(topic);
}

void Ab_MQTTClient::subscribeToTopics(const char* topics[]) {
  for (int i = 0; i < sizeof(topics) / sizeof(topics[0]); i++) {
    mqttClient.subscribe(topics[i]);
    Serial.print("Subscribed to topic: ");
    Serial.println(topics[i]);
  }
}
void Ab_MQTTClient::reconnect() {
    // Loop until reconnected
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (mqttClient.connect(id.c_str(), username, password)) {
            Serial.println("connected");
            
            attemptCount = 0;
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 2 seconds");
            attemptCount++;
            delay(2000);
        }

        if (attemptCount > 90) {
            Serial.println(attemptCount);
            Serial.println("restarting ESP32...");
            ESP.restart();
        }
    }
}

void Ab_MQTTClient::handleAuthenticationTopic(char* topic, byte* payload, unsigned int length) {
    // Deserialize the JSON message
    DynamicJsonDocument jsonDoc(256);
    DeserializationError error = deserializeJson(jsonDoc, payload, length);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
  
    // Extract the values from the JSON message
    const char* title = jsonDoc["title"];
    const char* message = jsonDoc["message"];
    const char* level = jsonDoc["level"];
    const char* vehicle = jsonDoc["vehicle"];
    const char* firstName = jsonDoc["first_name"];
    const char* lastName = jsonDoc["last_name"];
    const char* employeeId = jsonDoc["employee_id"];
    const char* userType = jsonDoc["type"];
    const char* unit = jsonDoc["unit"];
  
    // Perform actions based on the extracted values
    // For example, you can print them or use them in your application logic
    Serial.println("Authentication Topic Message:");
    Serial.print("Title: ");
    Serial.println(title);
    Serial.print("Message: ");
    Serial.println(message);
    Serial.print("Level: ");
    Serial.println(level);
    Serial.print("Vehicle: ");
    Serial.println(vehicle);
    Serial.print("First Name: ");
    Serial.println(firstName);
    Serial.print("Last Name: ");
    Serial.println(lastName);
    Serial.print("Employee ID: ");
    Serial.println(employeeId);
    Serial.print("User Type: ");
    Serial.println(userType);
    Serial.print("Unit: ");
    Serial.println(unit);
}
// Callback function for topic "other/topic"
void handleOtherTopic(char* topic, byte* payload, unsigned int length) {
    // Handle the other topic here
    // Extract the values from the JSON payload and perform necessary actions
}

void Ab_MQTTClient::callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
  
    const char* authenticationTopic = mqttcallback.authentication_topic.c_str();
    if (strcmp(topic, authenticationTopic) == 0) {
        handleAuthenticationTopic(topic, payload, length);
    } else if (strcmp(topic, "other/topic") == 0) {
        handleOtherTopic(topic, payload, length);
    }
}