#include "configs.h"
#include <Arduino.h>
#include <Ab_GPS.h>
#include <Ab_MQTTClient.h>
#include <Ab_RIDParameters.h>

RIDParameters rid;


//RID
//uint8_t id_type;
//uint8_t ua_type;
//String uas_id;
//uint8_t operator_id_type;
//String operator_id;
//uint8_t description_type;
//String description;
//uint8_t status;
//float direction;
//float speed_horizontal;
//float speed_vertical;
//float latitude;
//float longitude;
//float altitude_barometric;
//float altitude_geodetic;
//uint8_t height_type;
//float height;
//uint8_t horizontal_accuracy;
//uint8_t vertical_accuracy;
//uint8_t baro_alt_accuracy;
//uint8_t speed_accuracy;
//unsigned long timestamp;

// MQTT details
String        id               = BOX_ID;
const char*   mqtt_id          = MQTT_CLIENT_ID;
unsigned int  MQTT_interval    = MQTT_TIMEOUT;
unsigned int  port             = MQTT_HOST_PORT;
const char*   broker           = MQTT_GATEWAY;
const char*   mqttUsername     = MQTT_USER;
const char*   mqttPassword     = MQTT_PASS;
const char*   topicOutput1     = MQTT_TOPIC;
Ab_MQTTClient mqttClient(mqtt_id, broker, port, mqttUsername, mqttPassword);

mqttFields mqttCallback;
TaskHandle_t sensors;
TaskHandle_t network;

unsigned long lastReconnectAttempt = 0;
unsigned long t = millis();

long lastMsg = 0;


void setup() {
  Serial.begin(115200);
  rid.initialize();
  mqttClient.begin();
  //  xTaskCreatePinnedToCore(
  //    Sensors,   /* Task function. */
  //    "sensors",     /* name of task. */
  //    100000,       /* Stack size of task */
  //    NULL,        /* parameter of the task */
  //    1,           /* priority of the task */
  //    &sensors,      /* Task handle to keep track of created task */
  //    1);          /* pin task to core 0 */
  //  delay(2000);

  while (!GPS.begin()) {
    Serial.println("GPS setup fail");
    delay(100);
  }

  rid.id_type = "0";
  rid.ua_type = "0";
  rid.uas_id = "uas_id";
  rid.operator_id_type = "0";
  rid.operator_id = "";
  rid.description_type = "0";
  rid.description = "";
  rid.status = "0";
  rid.direction = "361.00";
  rid.speed_horizontal = "";
  rid.speed_vertical = "";
  rid.latitude = "0";
  rid.longitude = "0";
  rid.altitude_barometric = "-1000";
  rid.altitude_geodetic = "";
  rid.height_type = "2";
  rid.height = "-1000";
  rid.horizontal_accuracy = "0";
  rid.vertical_accuracy = "0";
  rid.baro_alt_accuracy = "0";
  rid.speed_accuracy = "0";
  rid.timestamp = "";
}

//void Sensors( void * pvParameters ) {
//
//  for (;;) {
//
//    long now = millis();
//
//    delay(10);
//  }
//}

void loop() {

  if (!mqttClient.isConnected()) {
    Serial.println("MQTT NOT CONNECTED!");
    mqttClient.reconnect();
  } else {
    long now = millis();

    if (MQTT_interval != MQTT_TIMEOUT) {
      MQTT_interval = MQTT_TIMEOUT;
      lastMsg = 0 ;
      Serial.println("ACTIVE MODE<10s>");
    }


    if (now - lastMsg > MQTT_interval || lastMsg == 0)
    {

      lastMsg = now;

      if (GPS.available()) {
        rid.latitude     = String(GPS.latitude(), 6);
        rid.longitude    = String(GPS.longitude(), 6);
        rid.speed_horizontal = String(GPS.speed(), 2);
        rid.height = String(GPS.altitude(), 1);
        rid.direction = String(GPS.course(), 2);
        rid.timestamp = String(GPS.getTime());
      } else {
        rid.latitude = "";
        rid.longitude = "";
        GPS.wakeup();
      }
      const char* mqttPayload = rid.getMqttPayload().c_str();
      mqttClient.publish("rid", mqttPayload);
      Serial.println(mqttPayload);
      delay(100);
    }
  }
  mqttClient.loop();
}
