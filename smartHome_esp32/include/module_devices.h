#ifndef _MODULE_DEVICES_H_
#define _MODULE_DEVICES_H_
#include <Arduino.h>

// wifi连接超时
#define CONNECTTIMEOUT 10
// 雨滴检测时长
#define RAINTIME 2000
// 感应灯时长
#define AUTOLAMPTIME 3000
// 检测结果保存时长
#define DETECTIONTIME 3000

// RGBLED灯
#define AUTOLED_PIN 19
#define LED_PIN 14

// 板载灯
#define ONBOARDLAMPPIN 2

// 不能 32 33 12 0 2 15 5
// 人体红外传感器
#define PIR_PIN 36
#define PIR_CHANNL A0

// 声音传感器
#define SOUND_PIN 34
#define SOUND_CHANNL A6

// 雨滴传感器
#define RAIN_PIN 35
#define RAIN_CHANNL A7

// sg90舵机
#define SG90_PIN 18

// 按键
#define BUTTON_PIN1 13
#define BUTTON_PIN2 4

void led_on();
void led_off();
void blinkLED(int n, int t);

void autoled_on();
void autoled_off();
int autoled_state();

void led3w_on();
void led3w_off();
int led3w_state();

void sg90_setAngle(int angle);

void startSensorTask();

void initDevices();

#endif