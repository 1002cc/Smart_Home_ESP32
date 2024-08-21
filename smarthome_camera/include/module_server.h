#ifndef _MODULE_SERVER_H_
#define _MODULE_SERVER_H_

#include <Arduino.h>

void printLocalTime(void);
void timeAvailable(struct timeval *t);
void initNtpTime();

void initWedServer();
void cameraserver_task(void *pvParameter);

void StoreData(const char *key, const char *val);
String ReadData(const char *val);

#endif