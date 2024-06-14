#ifndef _MODULE_SERVICE_H_
#define _MODULE_SERVICE_H_

#include <Arduino.h>
extern "C" {
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
}

void printLocalTime();
void ntpTask(void *param);
void ntpTimerCallback(TimerHandle_t xTimer);
void startNtpTask();

#endif