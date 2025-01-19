#ifndef _MODULE_SERVCE_H_
#define _MODULE_SERVCE_H_
#include <Arduino.h>

// MAX98357
#define PIN_I2S_MAX98357_BCLK 22
#define PIN_I2S_MAX98357_LRC 23
#define PIN_I2S_MAX98357_DOUT 21

enum class AUDIO_NAME {
    CONNECT_Y, // 连接成功
    CONNECT_N, // 连接失败
    CONNECT_D, // 断开连接
    PW,        // 配网
    WC,        // 欢迎语
    RAIN,      // 雨滴
    LT,        // 长时间停留
    PA,        // 入侵报警
    DC1,       // 门窗已打开
    DC12,      // 门窗已关闭
    DC2,       // 检测到门窗长时间未关闭
    DC3,       // 警报警报，非法闯入
    BL,        // 当前为蓝牙模式，如需网络服务请重启设备配置
    OTA,       //  OTA升级
};

enum ConnectionMode {
    MODE,
    WIFI,
    BLE,
    WB
};

void printLocalTime(void);
void timeAvailable(struct timeval *t);
void initNtpTime();

void StoreData(const char *key, const char *val);
String ReadData(const char *val);
void StoreintData(const char *key, int val);
int ReadintData(const char *val);

void printPSRAM(void);

void littlefs_init();

void audio_init();
void startAudioTack();
void audioPause();
void audioSpeak(const String &text);
void playAudio(const AUDIO_NAME &index);

String getAccessToken();
bool getOTAVersion();
void startOTA();
void startOTATask();

#endif