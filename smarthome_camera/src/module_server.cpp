#include "module_server.h"
#include "esp_camera.h"
#include "module_mqtt.h"
#include "module_wifi.h"
#include "sntp.h"
#include "time.h"
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
#include <HTTPUpdate.h>
#include <Preferences.h>

const char *g_ntp_server1 = "ntp.aliyun.com";
const char *g_ntp_server2 = "stdtime.gov.hk";
const long g_gmt_offset_sec = 3600;
const int g_daylight_offset_sec = 3600;
const char *g_time_zone = "CST-8"; // TimeZone rule for China Standard Time (UTC+8)
struct tm g_time;

String websockets_server_host = "47.115.139.166";
const uint16_t websockets_server_port = 3000;
using namespace websockets;
WebsocketsClient cameraClient;

Preferences preferences;

bool enableVideoSteam = false;
bool mqttControl = false;
bool videoStreamEnable = false;

TaskHandle_t cameraServerTaskHandle = NULL;

String FirmwareVersion = "1.0";
String latestFirmware = "1.0";
String FirmwareUrlCheck = "http://47.115.139.166:3005/firmware_lists/esp32cam";
String FirmwareUrl = "http://47.115.139.166:3005/download/esp32cam/firmwareV";

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

/********************************************************************
                         CAMERA WEDSERVER
********************************************************************/

void webSocketEvent(WebsocketsEvent event, WSInterfaceString data)
{
    if (event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connection Opened");
        Serial.println(data);
        sendCameraState(true);
        StoreData("cameraip", websockets_server_host.c_str());
    } else if (event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connection Closed");
        sendCameraState(false);
    } else if (event == WebsocketsEvent::GotPong) {
        Serial.println("Got Pong");
    } else if (event == WebsocketsEvent::GotPing) {
        Serial.println("Got Ping");
    }
}

void onMessageCallback(WebsocketsMessage message)
{
    if (message.length() > 0) { // 检查数据是否非空
        Serial.print("Got Message: ");
        Serial.println(message.data());
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, message.data());
        if (error) {
            Serial.print("Failed to parse JSON, error: ");
            Serial.println(error.c_str());
            return;
        }

        enableVideoSteam = doc["enableVideoSteam"];
        Serial.println(enableVideoSteam);
    } else {
        Serial.println("Received empty or null message.");
    }
}

void initWedServer()
{
    Serial.println("initWedServer");
    if (!wifitate()) {
        Serial.printf("initWedServer: wifitate=%d, cameraClient.available=%d\n", wifitate(), cameraClient.available());
        return;
    }

    cameraClient.onEvent(webSocketEvent);
    cameraClient.onMessage(onMessageCallback);

    Serial.println("connect" + websockets_server_host);
    while (!cameraClient.connect(websockets_server_host.c_str(), websockets_server_port, "/")) {
        delay(500);
        Serial.print(".");
    }

    int value = ReadintData("S");
    if (value != 1000) {
        videoStreamEnable = value;
        Serial.printf("videoStreamEnable=%d\n", videoStreamEnable);
    }
    pulishAllDatas();

    cameraClient.send("{camera:true}");
    Serial.println("server connect successful");
}

void cameraserver_task(void *pvParameter)
{
    Serial.println("camera server task start");
    unsigned long printfFB = 0;
    while (1) {
        if (!cameraClient.available()) {
            Serial.println("Connection lost, attempting to reconnect...");
            while (!cameraClient.connect(websockets_server_host.c_str(), websockets_server_port, "/")) {
                delay(500);
                Serial.print(".");
            }
            Serial.println("Reconnected successfully");
        } else {
            cameraClient.poll();

            if (enableVideoSteam || mqttControl || videoStreamEnable) {
                camera_fb_t *fb = esp_camera_fb_get();
                if (!fb) {
                    Serial.println("Camera capture failed");
                    esp_camera_fb_return(fb);
                }

                if (millis() - printfFB > 10 * 1000) {
                    printfFB = millis();
                    Serial.println("Camera capture success");
                    Serial.println(fb->len);
                }

                //  发送图片
                cameraClient.sendBinary((const char *)fb->buf, fb->len);

                // 返回内存
                esp_camera_fb_return(fb);

                if (!enableVideoSteam && !mqttControl && videoStreamEnable) {
                    vTaskDelay(1000);
                }
            }
        }
        // vTaskDelay(100);
        delay(100);
    }
    vTaskDelete(NULL);
}

void startCameraServerTask()
{
    // 开始视频流服务
    xTaskCreatePinnedToCore(cameraserver_task, "cameraserver_task", 5 * 1024, NULL, 5, &cameraServerTaskHandle, 1);
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
    vTaskResume(cameraServerTaskHandle);
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
    vTaskSuspend(cameraServerTaskHandle);
    xTaskCreatePinnedToCore(startOTA, "startOTA", 10000, NULL, 1, NULL, 0);
}
