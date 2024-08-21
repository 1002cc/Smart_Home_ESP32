#ifndef _MODULE_SERVCE_H_
#define _MODULE_SERVCE_H_
#include <Arduino.h>

void printLocalTime(void);
void timeAvailable(struct timeval *t);
void initNtpTime();

void StoreData(const char *key, const char *val);
String ReadData(const char *val);

void printPSRAM(void);

#endif