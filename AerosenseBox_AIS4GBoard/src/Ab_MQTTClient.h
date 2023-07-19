#ifndef Ab_MQTTClient_h
#define Ab_MQTTClient_h

#include <GSMClient.h>
#include <PubSubClient.h>
#include <SIM76xx.h>
#include <ArduinoJson.h>
struct mqttFields { 
    //--------authentication
    String authentication_topic;
    String authentication_vehicle;
    String authentication_first_name;
    String authentication_last_name;
    String authentication_employee_id;
    String authentication_unit;
    //-----------------------
    String callback_Username;
    String callback_Flight;
    String callback_STD;
    String callback_Bay;
    String callback_ETD;
    String callback_Gate;
    String callback_Job;
    
    //-------3
    uint8_t callback_maxJob =0;
    uint8_t callback_currentJob =0;
};

class Ab_MQTTClient {
public:
    Ab_MQTTClient(const char* id, const char* broker, int port, const char* username, const char* password);
    void begin();
    void loop();
    void publish(const char* topic, const char* payload);
    bool isConnected();
    void reconnect();	
    void subscribeToTopics(const char* topics[]);
    void subscribe(const char* topic); 
    mqttFields mqttcallback;
private:
    GSMClient gsmClient;
    PubSubClient mqttClient;
    const String id;
    const char* broker;
    int port;
    const char* username;
    const char* password;
    unsigned int attemptCount = 0;
    bool connected;
    StaticJsonDocument<200> jsonDoc;
    void callback(char* topic, byte* payload, unsigned int length);
    void handleAuthenticationTopic(char* topic, byte* payload, unsigned int length);
};

#endif
