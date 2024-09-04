#ifndef _MODULE_SPEAK_H_
#define _MODULE_SPEAK_H_

bool initI2SConfig();
void initSpeakConfig();
void startSpeakTask();
void stopSpeakTask();
void speakloop();

int speakPerid(int num);

#endif