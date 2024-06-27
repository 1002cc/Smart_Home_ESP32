#include "module_service.h"
#include "lvglconfig.h"
#include "wificonfig.h"
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <cJSON.h>

const char *ntpServer1 = "ntp.org";
const char *ntpServer2 = "ntp.ntsc.ac.cn";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

String weatherApiUrl = "http://api.seniverse.com/v3/weather/now.json?key=Srhi2-y2LtemJAmOB&location=guangzhou&language=zh-Hans&unit=c";
TaskHandle_t ntpxHandle = NULL;
TimerHandle_t ntpTimer;
void ntpTimerCallback(TimerHandle_t xTimer)
{
    // getDateTime();
    if (ntpxHandle == NULL) {
        Serial.println("Re-syncing time with NTP server");
        xTaskCreate(ntpTask, "NTP Task", 5 * 1024, NULL, 2, &ntpxHandle);
    }

    Serial.println("Update weather information");
    weatherQuery();
}

void ntpTask(void *param)
{
    int timenumber = 3;

    while (true) {
        if (!getwifistate()) {
            vTaskDelete(ntpxHandle);
            ntpxHandle = NULL;
        }
        configTime(60 * 60 * 8, 0, "ntp1.aliyun.com", "ntp2.aliyun.com", "cn.pool.ntp.org");

        struct tm timeinfo;
        if (getLocalTime(&timeinfo, 10000)) {
            Serial.println("Time synchronized successfully");
            // ui_calender_update();
            vTaskDelete(ntpxHandle);
            ntpxHandle = NULL;
        } else {
            Serial.println("Failed to synchronize time. Retrying...");
            timenumber += 10;
            if (timenumber > 120) {
                vTaskDelete(ntpxHandle);
                ntpxHandle = NULL;
            }
        }
        vTaskDelay(timenumber * 1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

bool isNetworkAvailable()
{
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }

    const IPAddress remoteIp(180, 76, 76, 76);

    WiFiClient client;
    const int timeoutMs = 2000;
    bool result = client.connect(remoteIp, 80, timeoutMs);
    delay(10);
    if (result) {
        client.stop();
        return true;
    }
    return false;
}

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

void startNTPTask(void)
{
    setup_ntp_client();
#if 1
    Serial.println("Starting NTP task");
    ntpTimer = xTimerCreate("NTP and Weather Timer", pdMS_TO_TICKS(300000), pdTRUE, (void *)0, ntpTimerCallback);
    if (ntpTimer == NULL) {
        Serial.println("Error xTimerCreate");
    } else {
        Serial.println("Starting NTP task successful");
        xTimerStart(ntpTimer, 0);
        ntpTimerCallback(NULL);
    }
#else
    getDateTime();
    Serial.println("Update weather information");
    weatherQuery();
#endif
}

void setup_ntp_client()
{
    timeClient.begin();
    // 设置时区
    // GMT +1 = 3600
    // GMT +8 = 28800
    // GMT -1 = -3600
    // GMT 0 = 0
    timeClient.setTimeOffset(28800);
}
String getDateTime()
{
    // 请求网络时间
    timeClient.update();

    unsigned long epochTime = timeClient.getEpochTime();
    Serial.print("Epoch Time: ");
    Serial.println(epochTime);

    String timeString = unixTimeToGMTString(epochTime);

    // 打印结果
    Serial.println(timeString);
    return timeString;
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
    time_t timer; // time_t就是long int 类型
    struct tm *tblock;
    timer = time(NULL);
    tblock = localtime(&timer);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", tblock);
    Serial.println(buffer);
    return String(buffer);
}