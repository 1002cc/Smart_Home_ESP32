#include "module_server.h"
#include "esp_camera.h"
#include "module_mqtt.h"
#include "module_wifi.h"
#include "sntp.h"
#include "time.h"
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
#include <Preferences.h>

const char *g_ntp_server1 = "ntp.aliyun.com";
const char *g_ntp_server2 = "stdtime.gov.hk";
const long g_gmt_offset_sec = 3600;
const int g_daylight_offset_sec = 3600;
const char *g_time_zone = "CST-8"; // TimeZone rule for China Standard Time (UTC+8)
struct tm g_time;

String websockets_server_host = "47.120.7.163";
const uint16_t websockets_server_port = 3000;
using namespace websockets;
WebsocketsClient cameraClient;

Preferences preferences;

static int printf_num = 0;

bool enableVideoSteam = false;
bool mqttControl = false;
bool videoStreamEnable = false;
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
                printf_num++;
                if (printf_num >= 200) {
                    printf_num = 0;
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
