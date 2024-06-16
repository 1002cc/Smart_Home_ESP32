#ifndef _MODULE_DEVICES_H_
#define _MODULE_DEVICES_H_
#include <Arduino.h>

#define USE_AUDIO 1
#define USE_INMP411 0

// RGB_LED
#define RGBLEDPIN 48

#define MQ2PIN 8
#define DHTPIN 7
#define DHTTYPE DHT11

#define PIN_R 35
#define PIN_G 36
#define PIN_B 37

// MAX98357
#define PIN_I2S_MAX98357_LRC 9
#define PIN_I2S_MAX98357_BCLK 10
#define PIN_I2S_MAX98357_DOUT 11

// INMP411
#define PIN_I2S_INMP411_WS 13
#define PIN_I2S_INMP411_SD 14
#define PIN_I2S_INMP411_SCK 21

void initWSrgbled();
void WSLED_Red();
void WSLED_Green();
void WSLED_Blue();
void WSLED_OFF();
void redled_on();
void redled_off();
void greenled_on();
void greenled_off();
void blueled_on();
void blueled_off();
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
bool getaudioPlayStatus();
#endif

void initmq2();
float readmq2();

void initDHT();
float dhtReadTemperature();
float dhtReadHumidity();

void readdataList();

void initDevices();

#endif