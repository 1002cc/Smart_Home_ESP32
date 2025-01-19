#include "module_server.h"
#include "module_mqtt.h"
#include "module_wifi.h"
#include "sntp.h"
#include "time.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <LittleFS.h>
#include <Preferences.h>
#include <audiohelpr.h>

AudioHelpr audio;
uint8_t cur_volume = 100;

const char *g_ntp_server1 = "ntp.aliyun.com";
const char *g_ntp_server2 = "stdtime.gov.hk";
const long g_gmt_offset_sec = 3600;
const int g_daylight_offset_sec = 3600;
const char *g_time_zone = "CST-8"; // TimeZone rule for China Standard Time (UTC+8)
struct tm g_time;

Preferences preferences;

extern TaskHandle_t devicesTaskHandle;

String FirmwareVersion = "1.0";
String latestFirmware = "1.0";
String FirmwareUrlCheck = "http://47.115.139.166:3005/firmware_lists/esp32wroom";
String FirmwareUrl = "http://47.115.139.166:3005/download/esp32wroom/firmwareV";

/********************************************************************
                         NTP SERVER
*********************************************************************/
void printLocalTime(void)
{
    if (!getLocalTime(&g_time)) {
        Serial.println("No time available (yet)");
        return;
    }
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

/********************************************************************
                         flash存储本地数据
*********************************************************************/
void littlefs_init()
{
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
    }
    Serial.println("LittleFS init succesful");
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

/********************************************************************
                         audio
********************************************************************/

void audio_init()
{
    audio.setPinout(PIN_I2S_MAX98357_BCLK, PIN_I2S_MAX98357_LRC, PIN_I2S_MAX98357_DOUT);
    audio.setVolume(cur_volume);
}

void audioPause()
{
    audio.stopSong();
    Serial.println(audio.isRunning());
}

void audioTask(void *pt)
{
    Serial.println("start audioTask");
    while (1) {
        audio.loop();
        vTaskDelay(5);
    }
    vTaskDelete(NULL);
}

void audioSpeak(const String &text)
{
    if (wifitate()) {
        audio.connecttospeech(text.c_str(), "zh");
    }
}

void startAudioTack()
{
    // 初始化音频模块
    audio_init();
    // 创建音频任务
    xTaskCreatePinnedToCore(audioTask, "audio_task", 1024 * 3, NULL, 2, NULL, 1);
}

void playAudio(const AUDIO_NAME &index)
{
    switch (index) {
    case AUDIO_NAME::CONNECT_Y:
        audio.connecttoFS(LittleFS, "/connect_y.mp3");
        break;
    case AUDIO_NAME::CONNECT_D:
        audio.connecttoFS(LittleFS, "/connect_d.mp3");
        break;
    case AUDIO_NAME::CONNECT_N:
        audio.connecttoFS(LittleFS, "/connect_n.mp3");
        break;
    case AUDIO_NAME::RAIN:
        audio.connecttoFS(LittleFS, "/rain.mp3");
        break;
    case AUDIO_NAME::PW:
        audio.connecttoFS(LittleFS, "/pw.mp3");
        break;
    case AUDIO_NAME::WC:
        audio.connecttoFS(LittleFS, "/wc.mp3");
        break;
    case AUDIO_NAME::LT:
        audio.connecttoFS(LittleFS, "/LT.mp3");
        break;
    case AUDIO_NAME::PA:
        audio.connecttoFS(LittleFS, "/PA.mp3");
        break;
    case AUDIO_NAME::DC1:
        audio.connecttoFS(LittleFS, "/dc1.mp3");
        break;
    case AUDIO_NAME::DC12:
        audio.connecttoFS(LittleFS, "/dc12.mp3");
        break;
    case AUDIO_NAME::DC2:
        audio.connecttoFS(LittleFS, "/dc2.mp3");
        break;
    case AUDIO_NAME::DC3:
        audio.connecttoFS(LittleFS, "/dc3.mp3");
        break;
    case AUDIO_NAME::BL:
        audio.connecttoFS(LittleFS, "/BL.mp3");
        break;
    case AUDIO_NAME::OTA:
        audio.connecttoFS(LittleFS, "/ota.mp3");
        break;
    default:
        break;
    }
}

/********************************************************************
                         ota升级
********************************************************************/

// 当升级开始时，打印日志
void update_started()
{
    Serial.println("CALLBACK:  HTTP update process started");
}

// 当升级结束时，打印日志
void update_finished()
{
    FirmwareVersion = latestFirmware;
    StoreData("Version", FirmwareVersion.c_str());
    // vTaskResume(speakTaskHandle);
    delay(600);
    Serial.println("CALLBACK:  HTTP update process finished");
}

// 当升级中，打印日志
void update_progress(int cur, int total)
{
    Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes[%.1f%%]...\n", cur, total, cur * 100.0 / total);
}

// 当升级失败时，打印日志
void update_error(int err)
{
    vTaskResume(devicesTaskHandle);
    Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

t_httpUpdate_return updateBin(String update_url)
{
    Serial.println("start update");
    WiFiClient UpdateClient;

    httpUpdate.onStart(update_started);     // 当升级开始时
    httpUpdate.onEnd(update_finished);      // 当升级结束时
    httpUpdate.onProgress(update_progress); // 当升级中
    httpUpdate.onError(update_error);       // 当升级失败时

    t_httpUpdate_return ret = httpUpdate.update(UpdateClient, update_url.c_str());

    return ret;
}

bool getOTAVersion()
{
    HTTPClient http;
    http.begin(FirmwareUrlCheck);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(response);
        DynamicJsonDocument doc(512);
        deserializeJson(doc, response);
        if (doc.is<JsonArray>() && doc.size() > 0) {
            String latestVersion = "0.0";
            for (int i = 0; i < doc.size(); i++) {
                String fileName = doc[i].as<String>();
                int startIndex = fileName.indexOf("V");
                int endIndex = fileName.lastIndexOf(".bin");
                if (startIndex != -1 && endIndex != -1 && startIndex < endIndex) {
                    String firmwareVersion = fileName.substring(startIndex + 1, endIndex);
                    if (firmwareVersion.toFloat() > latestVersion.toFloat()) {
                        latestVersion = firmwareVersion;
                    }
                } else {
                    Serial.println("无法从文件名中提取有效版本号: " + fileName);
                }
            }
            if (latestVersion != "0.0" && latestVersion.toFloat() > FirmwareVersion.toFloat()) {
                latestFirmware = latestVersion;
                Serial.println("最新固件版本: V" + latestFirmware);
                return true;
            } else {
                Serial.println("未找到有效固件版本");
                return false;
            }
        } else {
            Serial.println("服务器返回数据格式不符合预期");
            return false;
        }
    } else {
        Serial.println("Error on HTTP request");
        return false;
    }
    http.end();
    return false;
}

void startOTA(void *pvParameter)
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    String ota_url = FirmwareUrl + latestFirmware + ".bin";
    Serial.println(ota_url);
    t_httpUpdate_return ret = updateBin(ota_url); // 开始升级
    switch (ret) {
    case HTTP_UPDATE_FAILED: // 当升级失败
        // vTaskResume(speakTaskHandle);
        Serial.println("[update] Update failed.");
        break;
    case HTTP_UPDATE_NO_UPDATES: // 当无升级
        Serial.println("[update] Update no Update.");
        break;
    case HTTP_UPDATE_OK: // 当升级成功
        Serial.println("[update] Update ok.");
        break;
    }
}

void startOTATask()
{
    vTaskSuspend(devicesTaskHandle);
    xTaskCreatePinnedToCore(startOTA, "startOTA", 10000, NULL, 1, NULL, 0);
}
