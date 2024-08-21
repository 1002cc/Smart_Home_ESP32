#include "module_devices.h"
#include "module_mqtt.h"
#include "module_server.h"
#include "module_wifi.h"

void setup()
{
    Serial.begin(115200);

    // 打印PSRAM
    printPSRAM();

    // 初始化文件系统
    littlefs_init();

    // 初始化传感器设备
    initDevices();

    // 开始音频任务
    startAudioTack();

    // 配置wifi
    connectToWiFi(CONNECTTIMEOUT);

    // 初始化mqtt
    initMQTTConfig();

    // 开始传感器任务
    startSensorTask();

    // 打印PSRAM
    printPSRAM();
}

void loop()
{
    mqttLoop();
    if (!wifitate()) {
        checkDNS_HTTP();
    }
    checkConnect(true);

    vTaskDelay(500);
}
