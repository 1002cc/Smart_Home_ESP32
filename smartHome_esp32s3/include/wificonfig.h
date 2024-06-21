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

static wifi_buf_t wifi_buf;
static Network_Status_t networkStatus = NONE;
static TaskHandle_t ntScanTaskHandler, ntConnectTaskHandler;
static SemaphoreHandle_t xnetworkStatusSemaphore = NULL;

bool initWIFIConfig(void);
bool getwifistate();
void wifiDisconnect(void);
void wifiConnector(wifi_buf_t wifi_buf);
void networkScanner();
#endif