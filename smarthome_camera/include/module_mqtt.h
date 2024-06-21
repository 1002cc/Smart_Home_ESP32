#ifndef _MODULE_CONFIG_H_
#define _MODULE_CONFIG_H_

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
bool subscribeMQTT(const char topic[]);
void publishSensorData(const SensorData &data);
void mqtt_disconnect(void);
bool getMqttStart();

#endif