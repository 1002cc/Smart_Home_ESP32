#ifndef _MODULE_CONFIG_H_
#define _MODULE_CONFIG_H_

#include <Arduino.h>
#include <cJSON.h>

struct SensorData {
    float temp;
    float humidity;
    float mq;
};

bool initMQTTConfig(void);
bool mqttconnect(void);
void mqttLoop(void);
bool publishMQTT(const char payload[]);
bool publishMQTT(const char payload[], unsigned int plength);
void mqtt_beginPublish(unsigned int plength, boolean retained);
void mqtt_print(const String &s);
void mqtt_endPublish();
bool subscribeMQTT(const char topic[]);
void mqtt_disconnect(void);
bool getMqttStart();

#endif