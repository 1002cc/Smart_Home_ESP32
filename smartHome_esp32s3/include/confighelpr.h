#ifndef _CONFIGHELPR_H_
#define _CONFIGHELPR_H_
#include <Arduino.h>
#include <vector>

#define USE_AUDIO 1
#define USE_MAX98357 1
#define USE_INMP411 1
#define MAX_MUSIC_NUM 15
#define NETWORK_TIMEOUT (10 * 1000)

// 讯飞模型id
// 语音
#define TTS_APPID "6501ee63"
#define TTS_APIKEY "02e3f5858bb8af38e05a157c1fcfb9a7"
#define TTS_SECRETKEY "M2VmZmRmZjA0ZTFlMTQ2NDBkYmI1ZmE1"
#define TTS_WED_API "wss://tts-api.xfyun.cn/v2/tts"
#define STT_WED_API "wss://iat-api.xfyun.cn/v2/iat"
#define MINIMAX_KEY "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJHcm91cE5hbWUiOiLpmYjlhYnlro8iLCJVc2VyTmFtZSI6IumZiOWFieWujyIsIkFjY291bnQiOiIiLCJTdWJqZWN0SUQiOiIxNzkzMjYyMTIwMDA4MTcyNDYwIiwiUGhvbmUiOiIxODkyNjI0NzExMiIsIkdyb3VwSUQiOiIxNzkzMjYyMTE5OTk5NzgzODUyIiwiUGFnZU5hbWUiOiIiLCJNYWlsIjoiIiwiQ3JlYXRlVGltZSI6IjIwMjQtMDYtMjIgMTU6NDc6MTYiLCJpc3MiOiJtaW5pbWF4In0.XCozhyQkIr1r-_JXamid9sOa4A83v9dTcsyHj5COIi1bCgrzD4ANb-ZpNBIRs_qmvx5mUobo9Q3u890JzDllimit15QiXhDVv8kV71jJcoW-i0CfXC2N5HagOVgIGpKi3CZ12X1c3szjcSjjFAseq1djFuzRp6lCwxtVrqCSJz8Nwx2FO_Q0ZdWKxso73i6MfgfJAkfLHqc87SFwZXmvun08JVZrgwlDOF3QqvXce002Tpa6h9d6y1dG_cs-hXS4Br31h-3W7g_JAfywOz5yVSLaXi5ghnRRHqTXohSSzhuA9ZcS4h0LDI81425GVbTmKwapu9Op26YV69hK6IPyFg"

void StoreData(const char *key, const char *val);
String ReadData(const char *val);
void StoreintData(const char *key, int val);
int ReadintData(const char *val);
void LittleFS_init();
bool readfsDirlist(std::vector<String> &musiclist);
String musicSubstring(String str);
String optionsGet(std::vector<String> musiclist);
void printPSRAM(void);
#endif