#ifndef _MODULE_AUDIO_H_
#define _MODULE_AUDIO_H_

#include "confighelpr.h"
#include <Arduino.h>

void audio_init();
void startAudioTack();
void audioVolume(int volume);
void audioStation(int station);
void audiosetStation(int station);
void audioPrevious();
void audioNext();
void audioPlay();
void playMusicUrl(const String &url);
void audioPause();
void audioSetPer(int per);
void playStartAudio();
void playMQAlarm();
void playWakeup();
void playOTA();
void playMY();
bool getaudioPlayStatus();
void audioLoop();

#endif