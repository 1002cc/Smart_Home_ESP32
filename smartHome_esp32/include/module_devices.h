#ifndef _MODULE_DEVICES_H_
#define _MODULE_DEVICES_H_
#include <Arduino.h>

#define USE_AUDIO 0
// RGB_LED
#define RPIN 22
#define GPIN 21
#define BPIN 12
#define ONBOARDLAMPPIN 2

// MAX98357
#define PIN_I2S_MAX98357_BCLK 26
#define PIN_I2S_MAX98357_LRC 27
#define PIN_I2S_MAX98357_DOUT 25

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

void initDevices();

#endif