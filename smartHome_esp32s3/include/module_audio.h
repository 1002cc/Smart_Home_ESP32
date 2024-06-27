#ifndef _MODULE_AUDIO_H_
#define _MODULE_AUDIO_H_

#include <Arduino.h>

#include "confighelpr.h"

#if USE_AUDIO
void startAudioTack();
void audioVolume(int volume);
void audioStation(int station);
void audiosetStation(int station);
void audioPrevious();
void audioNext();
void audioPlay();
void audioPause();
bool getaudioPlayStatus();
void audioLoop();
#endif

void initSpeakConfig();

#endif