#ifndef _MODULE_SPEAK_H_
#define _MODULE_SPEAK_H_

typedef enum {
    NO_DIALOGUE,
    RECORDING,
    RECORDED,
    WAITING,
    ANSWERING
} SpeakState_t;

bool initI2SConfig();
void initSpeakConfig();
void startSpeakTask();
void stopSpeakTask();
void speakloop();
#endif