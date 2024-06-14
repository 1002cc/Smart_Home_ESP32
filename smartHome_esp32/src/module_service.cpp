#include "module_service.h"
#include "module_devices.h"
#include "ui.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <cJSON.h>
#include <lvgl.h>

String weatherApiUrl = "http://api.seniverse.com/v3/weather/now.json?key=Srhi2-y2LtemJAmOB&location=guangzhou&language=zh-Hans&unit=c";

void ui_calender_update()
{
    time_t now;
    struct tm *timeinfo;

    time(&now);                 // 获取当前时间
    timeinfo = localtime(&now); // 将时间转化为本地时间

    lv_calendar_set_showed_date(ui_Calendar2, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1);
    lv_calendar_set_today_date(ui_Calendar2, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
    Serial.println("LVGL_Calender_Update Successfully");
}

void ntpTimerCallback(TimerHandle_t xTimer)
{
    Serial.println("Re-syncing time with NTP server");
    xTaskCreate(ntpTask, "NTP Task", 4096, NULL, 1, NULL);
    Serial.println("Update weather information");
    weatherQuery();
}

void ntpTask(void *param)
{
    while (true) {
        configTime(60 * 60 * 8, 0, "ntp1.aliyun.com", "ntp2.aliyun.com", "cn.pool.ntp.org");

        struct tm timeinfo;
        if (getLocalTime(&timeinfo, 10000)) {
            Serial.println("Time synchronized successfully");
            ui_calender_update();
            vTaskDelete(NULL);
        } else {
            Serial.println("Failed to synchronize time. Retrying...");
        }
        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
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
    if (cJSON_IsArray(results) && cJSON_GetArraySize(results) > 0) {
        cJSON *result = cJSON_GetArrayItem(results, 0);
        cJSON *now = cJSON_GetObjectItem(result, "now");

        const char *weather_text = cJSON_GetObjectItem(now, "text")->valuestring;
        const char *temperature_str = cJSON_GetObjectItem(now, "temperature")->valuestring;
        const char *code_str = cJSON_GetObjectItem(now, "code")->valuestring;
        int code_number = atoi(code_str);

        Serial.print("code_number: ");
        Serial.print(code_number);
        Serial.print(" Weather: ");
        Serial.print(weather_text);
        Serial.print(" Temperature: ");
        Serial.println(temperature_str);

        char weather_char[28];
        snprintf(weather_char, sizeof(weather_char), "%s 室外温度 :%s°C", weather_text, temperature_str);
        Serial.println(weather_char);
        lv_label_set_text(ui_weatherLabel, weather_char);

        if (code_number == 4 || code_number == 9) {
            lv_img_set_src(ui_weathericonImage, &ui_img_520433372);
        } else if (code_number == 0) {
            lv_img_set_src(ui_weathericonImage, &ui_img_1282432712);
        } else if (code_number == 5 || code_number == 8) {
            lv_img_set_src(ui_weathericonImage, &ui_img_31831977);
        } else if (code_number >= 10 && code_number <= 19) {
            lv_img_set_src(ui_weathericonImage, &ui_img_1809401540);
        } else {
            lv_img_set_src(ui_weathericonImage, &ui_img_520433372);
        }

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
