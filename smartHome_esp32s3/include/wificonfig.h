#ifndef _WIFICONFIG_H_
#define _WIFICONFIG_H_
#include <Arduino.h>

typedef struct {
    String ssid;
    String pass;
} wifi_buf_t;

typedef enum {
    NONE,
    NETWORK_SEARCHING,
    NETWORK_SEARCHED,
    NETWORK_CONNECTED,
    NETWORK_CONNECT_FAILED
} Network_Status_t;

bool initWIFIConfig(void);
bool getwifistate();
String getwifissid();
void wifiDisconnect(void);
void wifiConnector();
void networkScanner();
#endif