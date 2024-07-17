#include "module_server.h"
#include "esp_camera.h"
#include "sntp.h"
#include "time.h"
#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <Preferences.h>

const char *g_ntp_server1 = "ntp.aliyun.com";
const char *g_ntp_server2 = "stdtime.gov.hk";
const long g_gmt_offset_sec = 3600;
const int g_daylight_offset_sec = 3600;
const char *g_time_zone = "CST-8"; // TimeZone rule for China Standard Time (UTC+8)
struct tm g_time;

String websockets_server_host = "47.120.7.163";
String serverip = "";
const uint16_t websockets_server_port = 3000;
using namespace websockets;
WebsocketsClient client;

TaskHandle_t cameraHandle = NULL;
bool isStartCamera = false;

Preferences preferences;

extern int devices_num;

/********************************************************************
                         NTP SERVER
*****************************************   ***************************/
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

void StoreData(const char *key, const char *val)
{
    preferences.begin("config", false);
    preferences.putString(key, val);
    preferences.end();
}
String ReadData(const char *val)
{
    preferences.begin("config", false);
    String ret = preferences.getString(val, "null");
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
        StoreData("cameraip", websockets_server_host.c_str());
    } else if (event == WebsocketsEvent::ConnectionClosed) {
        isStartCamera = false;
        devices_num = 0;
        if (cameraHandle != NULL) {
            vTaskDelete(cameraHandle);
            cameraHandle = NULL;
        }
        Serial.println("Connection Closed, delete task");
    }
}

void initwedServer()
{
    String cameraip = ReadData("cameraip");
    if (cameraip != "null") {
        websockets_server_host = cameraip;
    }
    client.onEvent(webSocketEvent);
    if (!serverip.isEmpty()) {
        websockets_server_host = serverip;
    }

    while (!client.connect(websockets_server_host.c_str(), websockets_server_port, "/")) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("server connect successful");
}

void cameraserver_task(void *pvParameter)
{
    initwedServer();
    Serial.println("start cameraserver_task");
    while (1) {
        if (client.available()) {
            // 拍摄图片
            camera_fb_t *fb = esp_camera_fb_get();
            if (!fb) {
                Serial.println("Camera capture failed");
                break;
            }
            // Serial.println(fb->len);
            //  发送图片
            client.sendBinary((const char *)fb->buf, fb->len);

            // 返回内存
            esp_camera_fb_return(fb);
        }
        vTaskDelay(5);
    }
    vTaskDelete(NULL);
}

void startcameraTask(void)
{
    if (isStartCamera && cameraHandle == NULL) {
        xTaskCreatePinnedToCore(cameraserver_task, "cameraserver_task", 5 * 1024, NULL, 5, &cameraHandle, 1);
    } else if (!isStartCamera && cameraHandle != NULL && devices_num == 0) {
        client.close();
        Serial.println("vTaskDelete cameraHandle");
        vTaskDelete(cameraHandle);
        cameraHandle = NULL;
    }
}