#include "confighelpr.h"
#include "lvglconfig.h"
#include "module_audio.h"
#include "module_devices.h"
#include "module_mqtt.h"
#include "module_service.h"
#include "module_speak.h"
#include "wificonfig.h"
#include <Arduino.h>

void setup()
{
    // 初始化串口
    Serial.begin(115200);
    Serial.println("smarthome start-up");
    // 打印PSRAM信息
    printPSRAM();
    // 初始化文件系统
    LittleFS_init();
    // 初始化LVGL
    initLVGLConfig();
    // 初始化语音和音频
    initSpeakConfig();
    // 初始化设备
    initDevices();
    // 初始化WIFI
    initWIFIConfig();
    // 初始化MQTT
    initMQTTConfig();
    delay(500);
    // 加载主页
    lv_gohome();
    // 初始化NTP
    startNTPTask();
    printPSRAM();
}

void loop()
{
    mqttLoop();
    vTaskDelay(100 / portTICK_PERIOD_MS);
}
