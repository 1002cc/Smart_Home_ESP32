#ifndef _MODULE_SPEAK_H_
#define _MODULE_SPEAK_H_

#include <Arduino.h>

void initSpeakConfig();

void speakloop();
int speakPerid(int num);
String instructionRecognition(const String &command);
bool instructionRecognitionSign(int sign);

void voiceModuleSerialWrite(String data);

void sendSTTData();

String XF_wsUrl(const char *Secret, const char *Key, String request, String host);
void speakTask(void *pvParameter);

String postDouBaoAnswer(String *answerlist, int listnum);
String getMiniMaxAnswer(String inputText);
String getXunfeiAnswer(const String &inputText);
String getDoubaoAnswer(const String &inputText);

#endif