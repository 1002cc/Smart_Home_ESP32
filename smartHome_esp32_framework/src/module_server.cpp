#include "module_server.h"
#include "module_mqtt.h"
#include "module_wifi.h"
#include "sntp.h"
#include "time.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <Preferences.h>

const char *g_ntp_server1 = "ntp.aliyun.com";
const char *g_ntp_server2 = "stdtime.gov.hk";
const long g_gmt_offset_sec = 3600;
const int g_daylight_offset_sec = 3600;
const char *g_time_zone = "CST-8"; // TimeZone rule for China Standard Time (UTC+8)
struct tm g_time;

Preferences preferences;

/********************************************************************
                         NTP SERVER
*********************************************************************/
void printLocalTime(void)
{
    if (!getLocalTime(&g_time)) {
        Serial.println("No time available (yet)");
        return;
    }
    // Serial.println(&g_time, "%Y-%m-%d %H:%M:%S");
}

/*
时间获取回调
*/
void timeAvailable(struct timeval *t)
{
    Serial.println("Got time adjustment from NTP!");
    printLocalTime();
}

void initNtpTime()
{
    sntp_set_time_sync_notification_cb(timeAvailable); // 配置时间获取回调
    configTzTime(g_time_zone, g_ntp_server1, g_ntp_server2);
}

String ReadData(const char *val)
{
    preferences.begin("config", false);
    String ret = preferences.getString(val, "null");
    preferences.end();
    return ret;
}

void StoreData(const char *key, const char *val)
{
    preferences.begin("config", false);
    preferences.putString(key, val);
    preferences.end();
}

void StoreintData(const char *key, int val)
{
    preferences.begin("config", false);
    preferences.putInt(key, val);
    preferences.end();
}
int ReadintData(const char *val)
{
    preferences.begin("config", false);
    int ret = preferences.getInt(val, 1000);
    preferences.end();
    return ret;
}

void printPSRAM(void)
{
    Serial.println("-----------------------------printPSRAM-----------------------------");
    Serial.printf("Total heap: %d\n", ESP.getHeapSize());
    Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
    Serial.printf("Deafult free size: %d\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    Serial.printf("PSRAM free size: %d\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    Serial.printf("Flash size: %d bytes\n", ESP.getFlashChipSize());
    Serial.printf("Free_heap_size = %d\n", esp_get_free_heap_size());
    Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
    Serial.printf("sp_get_free_internal_heap_size = %ld\n", esp_get_free_internal_heap_size());
    Serial.println("-----------------------------printPSRAM-----------------------------");
}

void littlefs_init()
{
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
    }
    Serial.println("LittleFS init succesful");
}