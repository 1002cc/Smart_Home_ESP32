#ifndef _MODULE_DEVICES_H_
#define _MODULE_DEVICES_H_
#include <Arduino.h>

#define USE_AUDIO 1

// RGBLED灯
#define RPIN 14
#define GPIN 13
#define BPIN 12

// 板载灯
#define ONBOARDLAMPPIN 2

// 声音传感器
#define SOUND_PIN 27

// 雨滴传感器
#define RAIN_PIN 0
#define RAINADCPIN A0

// sg90舵机
#define SG90_PIN 25

// 距离传感器
#define PIR_PIN 4

// MAX98357
#define PIN_I2S_MAX98357_BCLK 22
#define PIN_I2S_MAX98357_LRC 21
#define PIN_I2S_MAX98357_DOUT 23

void led_on();
void led_off();
void rgbled_red();
void rgbled_green();
void rgbled_blue();
void rgbled_off();
void rgbled_setColor(int r, int g, int b);

#if USE_AUDIO

String musicSubstring(String str);
String optionsGet();
void startAudioTack();
void audioVolume(int volume);
void audioStation(int station);
void audioPrevious();
void audioNext();
void audioPlay();
void audioPause();
#endif

void sg90_setAngle(int angle);

void startRainTask();

void initDevices();

#endif