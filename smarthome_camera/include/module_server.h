#ifndef _MODULE_SERVER_H_
#define _MODULE_SERVER_H_

#include <Arduino.h>

void printLocalTime(void);
void timeAvailable(struct timeval *t);
void initNtpTime();

void initWedServer();
void startCameraServerTask();
;

void StoreData(const char *key, const char *val);
String ReadData(const char *val);
void StoreintData(const char *key, int val);
int ReadintData(const char *val);

String getAccessToken();
bool getOTAVersion();
void startOTA();
void startOTATask();

#endif