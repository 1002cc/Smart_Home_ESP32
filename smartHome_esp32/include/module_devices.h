#ifndef _MODULE_DEVICES_H_
#define _MODULE_DEVICES_H_
#include <Arduino.h>

// wifi连接超时
#define CONNECTTIMEOUT 10
// 雨滴检测时长
#define RAINTIME 2000
// 感应灯时长
#define AUTOLAMPTIME 10000

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

// MAX98357
#define PIN_I2S_MAX98357_BCLK 22
#define PIN_I2S_MAX98357_LRC 23
#define PIN_I2S_MAX98357_DOUT 21

#define MINIMAX_KEY "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJHcm91cE5hbWUiOiLpmYjlhYnlro8iLCJVc2VyTmFtZSI6IumZiOWFieWujyIsIkFjY291bnQiOiIiLCJTdWJqZWN0SUQiOiIxNzkzMjYyMTIwMDA4MTcyNDYwIiwiUGhvbmUiOiIxODkyNjI0NzExMiIsIkdyb3VwSUQiOiIxNzkzMjYyMTE5OTk5NzgzODUyIiwiUGFnZU5hbWUiOiIiLCJNYWlsIjoiIiwiQ3JlYXRlVGltZSI6IjIwMjQtMDYtMjIgMTU6NDc6MTYiLCJpc3MiOiJtaW5pbWF4In0.XCozhyQkIr1r-_JXamid9sOa4A83v9dTcsyHj5COIi1bCgrzD4ANb-ZpNBIRs_qmvx5mUobo9Q3u890JzDllimit15QiXhDVv8kV71jJcoW-i0CfXC2N5HagOVgIGpKi3CZ12X1c3szjcSjjFAseq1djFuzRp6lCwxtVrqCSJz8Nwx2FO_Q0ZdWKxso73i6MfgfJAkfLHqc87SFwZXmvun08JVZrgwlDOF3QqvXce002Tpa6h9d6y1dG_cs-hXS4Br31h-3W7g_JAfywOz5yVSLaXi5ghnRRHqTXohSSzhuA9ZcS4h0LDI81425GVbTmKwapu9Op26YV69hK6IPyFg"
#define MINIMAX_TTS "https://api.minimax.chat/v1/t2a_pro?GroupId=1793262119999783852"

// led1(不用)
// void rgbled_red();
// void rgbled_green();
// void rgbled_blue();
// void rgbled_off();
// void rgbled_setColor(int r, int g, int b);
// RGBLED
// void redled_on();
// void redled_off();
// void greenled_on();
// void greenled_off();
// void blueled_on();
// void blueled_off();

void led_on();
void led_off();
void blinkLED(int n, int t);

void autoled_on();
void autoled_off();
int autoled_state();

void led3w_on();
void led3w_off();
int led3w_state();

void startAudioTack();
void audioPause();

void audioSpeak(const String &text);

void sg90_setAngle(int angle);

void startSensorTask();

void initDevices();

#endif