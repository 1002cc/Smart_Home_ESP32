#include "module_service.h"
#include "confighelpr.h"
#include "lvglconfig.h"
#include "module_audio.h"
#include "wificonfig.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <cJSON.h>

const char *ntpServer1 = "ntp1.aliyun.com";
String ntpServer2 = "ntp2.aliyun.com";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
int updatetimentp = 360;

String weatherApiUrl = "http://api.seniverse.com/v3/weather/now.json?key=Srhi2-y2LtemJAmOB&location=guangzhou&language=zh-Hans&unit=c";
TaskHandle_t ntpxHandle = NULL;
TimerHandle_t ntpTimer;

const char *loginurl = "http://hichchen.top:3002/userlogin";
String upassword;
bool loginState = true;

const char *API_KEY = "P9SYyz2QOA4lsXjFLv8DRv8p";
const char *SECRET_KEY = "rKFhtw5D3H3pRUPY44jrvsb46Qr3prSr";

// 服务器端口 3000(http)  3003(https) 视频流服务器端口
// 服务器端口 3002(http)  3001(https) 数据库服务器端口
// 服务器端口 3005(http)  ota固件升级端口
String FirmwareVersion = "1.0";
String latestFirmware = "1.0";
String FirmwareUrlCheck = "http://47.115.139.166:3005/firmware_lists/esp32s3";
String FirmwareUrl = "http://47.115.139.166:3005/download/esp32s3/firmwareV";

extern TaskHandle_t devicesTaskHandle;
extern TaskHandle_t speakTaskHandle;

/********************************************************************
                         ntp服务
********************************************************************/
void ntpTimerCallback(TimerHandle_t xTimer)
{
    Serial.println("Re-syncing time with NTP server");
    xTaskCreatePinnedToCore(ntpTask, "NTP Task", 3 * 1024, NULL, 2, &ntpxHandle, 0);
    Serial.println("Update weather information");
    weatherQuery();
}

void ntpTask(void *param)
{
    int timenumber = 3;
    while (true) {
        if (!getwifistate()) {
            break;
        }
        configTime(60 * 60 * 8, 0, ntpServer1, ntpServer2.c_str(), "cn.pool.ntp.org");

        struct tm timeinfo;
        if (getLocalTime(&timeinfo, 10000)) {
            Serial.println("Time synchronized successfully");
            ui_calender_update();
            break;
        } else {
            Serial.println("Failed to synchronize time. Retrying...");
            timenumber += 10;
            if (timenumber > 120) {
                break;
            }
        }
        vTaskDelay(timenumber * 1000 / portTICK_PERIOD_MS);
    }
    Serial.println("delete ntptask");
    vTaskDelete(NULL);
}

void updateTimer()
{
    xTimerStop(ntpTimer, 0);
    xTimerChangePeriod(ntpTimer, updatetimentp * 1000, 0);
    xTimerReset(ntpTimer, 0);
}

void startNTPTask(void)
{
    Serial.println("Starting NTP task");
    ntpTimer = xTimerCreate("NTP and Weather Timer", pdMS_TO_TICKS(updatetimentp * 1000), pdTRUE, (void *)0, ntpTimerCallback);
    if (ntpTimer == NULL) {
        Serial.println("Error xTimerCreate");
    } else {
        Serial.println("Starting NTP task successful");
        xTimerStart(ntpTimer, 0);
        ntpTimerCallback(NULL);
    }
}

String unixTimeToGMTString(time_t unixTime)
{
    char buffer[80];
    struct tm timeinfo;
    gmtime_r(&unixTime, &timeinfo);
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", &timeinfo);
    Serial.println(buffer);
    return String(buffer);
}

String getDateTime_one()
{
    time_t timer;
    struct tm *tblock;
    timer = time(NULL);
    // tblock = localtime(&timer);
    tblock = gmtime(&timer);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", tblock);
    Serial.println(buffer);
    return String(buffer);
}

/********************************************************************
                         检测是否有网络
********************************************************************/
bool isNetworkAvailable()
{
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }

    const IPAddress remoteIp(180, 76, 76, 76);

    WiFiClient client;
    const int timeoutMs = 2000;
    bool result = client.connect(remoteIp, 80, timeoutMs);
    delay(100);
    if (result) {
        client.stop();
        return true;
    }
    return false;
}

/********************************************************************
                         天气服务
********************************************************************/
void parse_json(const char *json_string)
{
    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL) {
        Serial.println("Error parsing JSON");
        return;
    }

    cJSON *results = cJSON_GetObjectItem(root, "results");
    if (results != NULL && (results) && cJSON_GetArraySize(results) > 0) {
        cJSON *result = cJSON_GetArrayItem(results, 0);
        if (result == NULL) {
            Serial.println("Error parsing JSON");
            return;
        }
        cJSON *now = cJSON_GetObjectItem(result, "now");
        if (now == NULL) {
            Serial.println("Error parsing JSON");
            return;
        }
        const char *weather_text = cJSON_GetObjectItem(now, "text")->valuestring;
        const char *temperature_str = cJSON_GetObjectItem(now, "temperature")->valuestring;
        const char *code_str = cJSON_GetObjectItem(now, "code")->valuestring;
        int code_number = atoi(code_str);

        cJSON *result1 = cJSON_GetArrayItem(results, 0);
        if (result1 == NULL) {
            Serial.println("Error parsing JSON");
            return;
        }
        cJSON *location = cJSON_GetObjectItem(result1, "location");
        if (location == NULL) {
            Serial.println("Error parsing JSON");
            return;
        }
        const char *city_text = cJSON_GetObjectItem(location, "name")->valuestring;
        Serial.print("code_number: ");
        Serial.print(code_number);
        Serial.print(" Weather: ");
        Serial.print(weather_text);
        Serial.print(" Temperature: ");
        Serial.println(temperature_str);

        char weather_char[28];
        snprintf(weather_char, sizeof(weather_char), "%s 室外温度 :%s°C", weather_text, temperature_str);
        Serial.println(weather_char);
        lv_setWeatherinfo(weather_char);
        lv_setCityinfo(city_text);
        lv_setWeatherImage(code_number);
        cJSON_Delete(root);
    } else {
        Serial.println("Invalid or empty 'results' array");
    }
}

void updateCityID(String cityIDStr)
{
    weatherApiUrl = "http://api.seniverse.com/v3/weather/now.json?key=Srhi2-y2LtemJAmOB&location=" + cityIDStr + "&language=zh-Hans&unit=c";
}

void weatherQuery()
{
    HTTPClient http;
    http.begin(weatherApiUrl);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String responseBody = http.getString();
        parse_json(responseBody.c_str());
        Serial.println(responseBody);
    } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}

/********************************************************************
                         用户登录
********************************************************************/
bool postlogin(String username, String password)
{
    bool isLogin = false;
    HTTPClient http;
    http.begin(loginurl);
    http.addHeader("Content-Type", "application/json;charset=UTF-8");

    String data = "{\"username\":\"" + String(username) + "\",\"password\":\"" + String(password) + "\"}";

    Serial.println(http.getLocation());
    Serial.println(data);

    int httpCode = http.POST(data);
    if (httpCode > 0) {
        String response = http.getString();
        Serial.println(httpCode);
        Serial.println(response);
        cJSON *root = cJSON_Parse(response.c_str());
        if (root) {
            cJSON *loginSuccessItem = cJSON_GetObjectItem(root, "loginSuccess");
            if (loginSuccessItem && cJSON_IsBool(loginSuccessItem) && loginSuccessItem->valueint) {
                Serial.println("Login successful!");
                isLogin = true;
            } else {
                Serial.println("Login failed.");
            }
            cJSON_Delete(root);
        } else {
            Serial.println("Error parsing JSON.");
        }
    } else {
        Serial.println("Error on HTTP request");
    }
    http.end();
    return isLogin;
}

/********************************************************************
                         百度token获取
********************************************************************/
String getAccessToken()
{
    HTTPClient http;
    String url = "https://aip.baidubce.com/oauth/2.0/token";
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String params = "grant_type=client_credentials&client_id=" + String(API_KEY) + "&client_secret=" + String(SECRET_KEY);
    int httpResponseCode = http.POST(params);
    if (httpResponseCode > 0) {
        String response = http.getString();
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, response);
        return doc["access_token"].as<String>();
    } else {
        return "";
    }
    http.end();
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
    vTaskResume(devicesTaskHandle);
    vTaskResume(speakTaskHandle);
    delay(600);
    Serial.println("CALLBACK:  HTTP update process finished");
}

// 当升级中，打印日志
void update_progress(int cur, int total)
{
    lv_updataOATbar(cur * 100.0 / total);
    // Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes[%.1f%%]...\n", cur, total, cur * 100.0 / total);
}

// 当升级失败时，打印日志
void update_error(int err)
{
    vTaskResume(devicesTaskHandle);
    vTaskResume(speakTaskHandle);
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
        vTaskResume(devicesTaskHandle);
        vTaskResume(speakTaskHandle);
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
    vTaskSuspend(speakTaskHandle);
    xTaskCreatePinnedToCore(startOTA, "startOTA", 10000, NULL, 1, NULL, 0);
}
