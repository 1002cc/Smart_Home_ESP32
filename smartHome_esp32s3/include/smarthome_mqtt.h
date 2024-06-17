#ifndef SMARTHOME_MQTT_H
#define SMARTHOME_MQTT_H

#include <cJSON.h>

struct SensorData {
    float temp;
    float humidity;
    float mq;
};

bool initMQTT();
void mqttLoop();
void startMqttTask();
bool publishMQTT(const char payload[]);
bool subscribeMQTT(const char topic[]);
void publishSensorData(const SensorData &data);
void mqtt_disconnect();
#endif
