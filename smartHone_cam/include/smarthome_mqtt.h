#ifndef SMARTHOME_MQTT_H
#define SMARTHOME_MQTT_H

bool initMQTT();
void mqttloop();
bool publishMQTT(const char payload[]);
bool subscribeMQTT(const char topic[]);
#endif
