#include "module_service.h"

const char *ntpServer = "ntp1.aliyun.com";
TimerHandle_t ntpTimer;

void printLocalTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %Y-%m-%d %H:%M:%S");
}

void ntpTask(void *param)
{
    while (true) {
        configTime(60 * 60 * 8, 0, ntpServer, "ntp2.aliyun.com", "cn.pool.ntp.org");

        struct tm timeinfo;
        if (getLocalTime(&timeinfo, 10000)) {
            Serial.println("Time synchronized successfully");
            printLocalTime();
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
    xTaskCreate(ntpTask, "NTP Task", 4096, NULL, 2, NULL);
}

void startNtpTask()
{
    xTaskCreate(ntpTask, "NTP Task", 4096, NULL, 2, NULL);
    ntpTimer = xTimerCreate("NTP Timer", pdMS_TO_TICKS(3600000), pdTRUE, (void *)0, ntpTimerCallback);
    xTimerStart(ntpTimer, 0);
}