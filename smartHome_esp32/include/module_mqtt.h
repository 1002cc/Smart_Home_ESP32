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
void mqttMontage(const String &user);
void publishDeviceState();
void publishDeviceVersion();

void pulishState(const String &object, const bool &state, const String &item = "datas");
void pulishSwitchDatas(const lampButtonData &data);
void pulishAllSwitchDatas();

#endif