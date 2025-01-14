#ifndef _MODULE_SPEAK_H_
#define _MODULE_SPEAK_H_

#include <Arduino.h>

void initSpeakConfig();
void speakloop();
int speakPerid(int num);
String instructionRecognition(const String &command);
String instructionRecognitionSign(int sign);
void sendSTTData();
void llmRequest();
String wedUrlXF(const char *Secret, const char *Key, String request, String host);
void speakTask(void *pvParameter);
String postDouBaoAnswer(String *answerlist, int listnum);
String getMiniMaxAnswer(String inputText);
String getXunfeiAnswer(const String &inputText);
String getDoubaoAnswer(const String &inputText);
void SerialFlush();

// 更新百度合成taken
// String tok = getAccessToken();
// Serial.printf("token:%s\n", tok.c_str());
// if (tok.isEmpty()) {
//     tok = "";
//     tok = ReadData("tok");
//     if (tok != "null") {
//         audio.setTok(tok.c_str());
//     } else {
//         audio.setTok("24.fe14f4355b5db90310d8c6fd20394caf.2592000.1736132606.282335-109052759");
//     }
// } else {
//     StoreData("tok", tok.c_str());
//     audio.setTok(tok.c_str());
// }

#endif