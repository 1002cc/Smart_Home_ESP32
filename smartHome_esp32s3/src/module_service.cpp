#include "module_service.h"
#include "lvglconfig.h"
#include "wificonfig.h"
#include <HTTPClient.h>
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
String username;
String upassword;
bool loginState = false;
void ntpTimerCallback(TimerHandle_t xTimer)
{
    // if (ntpxHandle != NULL) {
    //     vTaskDelete(ntpxHandle);
    //     ntpxHandle = NULL;
    // }
    Serial.println("Re-syncing time with NTP server");
    xTaskCreatePinnedToCore(ntpTask, "NTP Task", 5 * 1024, NULL, 2, &ntpxHandle, 0);
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
    time_t timer; // time_t就是long int 类型
    struct tm *tblock;
    timer = time(NULL);
    tblock = localtime(&timer);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", tblock);
    Serial.println(buffer);
    return String(buffer);
}

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
void LoginRRequestTask(void *pvParameters)
{
    while (1) {
        if ((getwifistate())) {
            HTTPClient http;
            http.begin(loginurl);
            http.addHeader("Content-Type", "application/json;charset=UTF8");

            String data = "{\"username\":\"" + String(username) + "\",\"password\":\"" + String(upassword) + "\"}";

            int httpCode = http.POST(data);
            if (httpCode > 0) {
                String payload = http.getString();
                Serial.println(httpCode);
                Serial.println(payload);
            } else {
                Serial.println("Error on HTTP request");
            }
            http.end();
        }
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

void startLoginReequestTask(void)
{
    Serial.println("Starting Login Request task");
    xTaskCreatePinnedToCore(LoginRRequestTask, "httpRequestTask", 4096, NULL, 1, NULL, 0);
}
