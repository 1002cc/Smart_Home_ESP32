#ifndef _MODULE_CONFIG_H_
#define _MODULE_CONFIG_H_

#include <Arduino.h>
#include <cJSON.h>

struct SensorData {
    float temp;
    float humidity;
    float mq;
};

struct lampButtonData {
    bool lampButton1;
    bool lampButton2;
    bool lampButton3;
    bool lampButton4;
};

bool initMQTTConfig(void);
bool mqttconnect(void);
void mqttLoop(void);
bool publishMQTT(const char payload[]);
bool subscribeMQTT(const char topic[]);
void publishSensorData(const SensorData &data);
void pulishSwitchDatas(const lampButtonData &data);
void publishGetImage();
void publishStartVideo(bool isStartVideo);
void mqtt_disconnect(void);
bool getMqttStart();
void mqtMontage(const String &user);

#endif