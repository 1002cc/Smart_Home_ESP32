#ifndef _CONFIGHELPR_H_
#define _CONFIGHELPR_H_
#include <Arduino.h>
#include <vector>

#define MAX_MUSIC_NUM 15
#define NETWORK_TIMEOUT (5 * 1000)

// 讯飞模型id
// 语音

#define XFROLE "你是一个智能家居助手小欧，回答问题比较简洁。"
#define TTS_WED_API "ws-api.xfyun.cn"
#define STT_WED_API "wss://iat-api.xfyun.cn/v2/iat"
#define MINIMAX_KEY "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJHcm91cE5hbWUiOiLpmYjlhYnlro8iLCJVc2VyTmFtZSI6IumZiOWFieWujyIsIkFjY291bnQiOiIiLCJTdWJqZWN0SUQiOiIxNzkzMjYyMTIwMDA4MTcyNDYwIiwiUGhvbmUiOiIxODkyNjI0NzExMiIsIkdyb3VwSUQiOiIxNzkzMjYyMTE5OTk5NzgzODUyIiwiUGFnZU5hbWUiOiIiLCJNYWlsIjoiIiwiQ3JlYXRlVGltZSI6IjIwMjQtMDYtMjIgMTU6NDc6MTYiLCJpc3MiOiJtaW5pbWF4In0.XCozhyQkIr1r-_JXamid9sOa4A83v9dTcsyHj5COIi1bCgrzD4ANb-ZpNBIRs_qmvx5mUobo9Q3u890JzDllimit15QiXhDVv8kV71jJcoW-i0CfXC2N5HagOVgIGpKi3CZ12X1c3szjcSjjFAseq1djFuzRp6lCwxtVrqCSJz8Nwx2FO_Q0ZdWKxso73i6MfgfJAkfLHqc87SFwZXmvun08JVZrgwlDOF3QqvXce002Tpa6h9d6y1dG_cs-hXS4Br31h-3W7g_JAfywOz5yVSLaXi5ghnRRHqTXohSSzhuA9ZcS4h0LDI81425GVbTmKwapu9Op26YV69hK6IPyFg"
#define MINIMAX_TTS "https://api.minimax.chat/v1/t2a_pro?GroupId=1793262119999783852"

void StoreData(const char *key, const char *val);
String ReadData(const char *val);
void StoreintData(const char *key, int val);
int ReadintData(const char *val);
void LittleFS_init();
bool readfsDirlist(std::vector<String> &musiclist);
void appendToFile(const char *message, const char *path = "/musiclist.txt");
String musicSubstring(String str);
String optionsGet(std::vector<String> musiclist);
void printPSRAM(void);
#endif