#ifndef _MODULE_SERVICE_H_
#define _MODULE_SERVICE_H_
#include <Arduino.h>
extern "C" {
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
}

void weatherQuery();
void ntpTask(void *param);
void startNTPTask(void);
void ntpTimerCallback(TimerHandle_t xTimer);
bool isNetworkAvailable();
void updateCityID(String cityIDStr);
void parse_json(const char *json_string);
void updateTimer();
String unixTimeToGMTString(time_t unixTime);
String getDateTime_one();
bool postlogin(String username, String password);

String getAccessToken();
bool getOTAVersion();
void startOTA();
void startOTATask();

void sendSMSMessage(String messageContent);
#endif