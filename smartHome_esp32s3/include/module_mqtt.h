#ifndef _MODULE_CONFIG_H_
#define _MODULE_CONFIG_H_

#include <Arduino.h>
#include <cJSON.h>

struct SensorData {
    float temp;
    float humidity;
    float mq;
};

struct detectionDate {
    bool pri;
    bool rain;
    bool voice;
};

struct lampButtonData {
    bool lampButton1;
    bool lampButton2;
    bool priButton;
    bool voiceButton;
    bool fan;
    bool curtain;
    bool doorcontact;
    bool openSound;
    bool timeout;
    int timeoutTime;
};

bool initMQTTConfig(void);
bool mqttconnect(void);
void stopMQTT();
void mqttLoop(void);
bool publishMQTT(const char payload[]);
bool subscribeMQTT(const char topic[]);
void publishSensorData(const SensorData &data);
void publishGetDatas();
void publishDeviceState();
void publishDeviceVersion();
bool pulishSwitchDatas(const lampButtonData &data);
void mqtt_disconnect(void);
bool getMqttStart();
void mqtMontage(const String &user);
bool pulishState(const String &object, const bool &state, const String &item);
bool pulishState_int(const String &object, const int &num, const String &item);
void publishVideoAbout(int value);
bool sendRePW();
bool sendMQAlarm(const bool &state);

#endif