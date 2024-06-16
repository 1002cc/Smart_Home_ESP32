#include "Arduino.h"
#include "WiFi.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "module_camera.h"
#include "module_devices.h"
#include "module_service.h"
#include "module_wificonfig.h"
#include "smarthome_mqtt.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"
#include "time.h"
#include <nvs.h>
#include <nvs_flash.h>

bool PIR_On = false;

static unsigned long lastMotionTime = 0;
static bool ledOn = false;

#define DEGUB_ESP 1
#define WED_SERVER 1

#ifdef DEGUB_ESP
#define DBG(x) Serial.println(x)
#else
#define DBG(...)
#endif

// ntp

boolean takeNewPhoto = false;

#define FILE_PHOTO "/photo.jpg"

void handleMotion();

void handleMotion()
{
    // int pirState = digitalRead(PIR_PIN);
    int soundState = digitalRead(SOUND_PIN);
    if (soundState) {
        Serial.println("Motion detected soundState!");
        lastMotionTime = millis();
        if (!ledOn) {
            Serial.println("led open");
            ledOn = true;
            led_on();
        }
    }

    if (ledOn && millis() - lastMotionTime > 10000) {
        ledOn = false;
        led_off();
        Serial.println("LED auto turned off");
    }

    // Serial.println(analogRead(PIR_PIN));
    int pirState = digitalRead(PIR_PIN);
    if (pirState) {
        rgbled_red();
        Serial.println("Motion detected ");
    } else {
        rgbled_green();
        Serial.println("no Motion ");
    }
}
WiFiUser wifiUser;
void setup()
{
    Serial.begin(115200);

    // device_init();
    // connectToWiFi();
    Serial.print("IP Address: http://");
    Serial.println(WiFi.localIP());

    // 初始化相机
    initCamera();

    // mqtt服务
    initMQTT();

    startNtpTask();
    Serial.print("-------------------");
    esp_wifi_restore();
}

void loop()
{
    mqttloop();
    handleMotion();

    wifiUser.checkConnect(true);
    // 处理DNS和HTTP请求
    wifiUser.checkDNS_HTTP();
    vTaskDelay(1000);
}
