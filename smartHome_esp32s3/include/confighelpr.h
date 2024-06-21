#ifndef _CONFIGHELPR_H_
#define _CONFIGHELPR_H_
#include <Arduino.h>
#include <vector>

#define USE_AUDIO 1
#define USE_INMP411 0
#define MAX_MUSIC_NUM 15
#define NETWORK_TIMEOUT (10 * 1000)

static bool hasNetwork;
static bool enable_mqtt = false;

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