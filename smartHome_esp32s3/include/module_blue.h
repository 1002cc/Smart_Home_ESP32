#ifndef _MODULE_BLUE_H_
#define _MODULE_BLUE_H_
#include <arduino.h>

#define USE_BLE 0

#if USE_BLE

void initBLE();
void startBLE();
void stopBLE();
void BLELoop();
void sendIntDataBLE(int num);
void sendStrDataBLEStr(const String &str);

// 离线语音控制
// if (!isOnlineMode && convertedInt != 1) {
//     if (instructionRecognitionSign(convertedInt)) {
//         Serial.printf("send command to voice module: %d\n", convertedInt);
//         voiceModuleSerial.print(convertedInt);
//     }
// }

#endif

#endif