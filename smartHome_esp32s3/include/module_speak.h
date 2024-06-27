#ifndef _MODULE_SPEAK_H_
#define _MODULE_SPEAK_H_

typedef enum {
    SPEAKNONE,
    RECORDING,
    REQUESTING,
    SPEAKING
} SpeakState_t;

bool initI2SConfig();
void initSpeakConfig();
void startSpeakTask();
void stopSpeakTask();
void speakloop();

#endif