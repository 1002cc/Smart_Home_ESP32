#include "module_service.h"
#include "module_devices.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include <Preferences.h>

void ntpTask(void *param)
{
    while (true) {
        configTime(60 * 60 * 8, 0, "ntp1.aliyun.com", "ntp2.aliyun.com", "cn.pool.ntp.org");

        struct tm timeinfo;
        if (getLocalTime(&timeinfo, 10000)) {
            Serial.println("Time synchronized successfully");
            vTaskDelete(NULL);
        } else {
            Serial.println("Failed to synchronize time. Retrying...");
        }
        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
}

void ntpTimerCallback(TimerHandle_t xTimer)
{
    Serial.println("Re-syncing time with NTP server");
    xTaskCreate(ntpTask, "NTP Task", 4096, NULL, 1, NULL);
    Serial.println("Update weather information");
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
