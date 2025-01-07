#include "esp_camera.h"
#include "module_camera.h"
#include "module_devices.h"
#include "module_mqtt.h"
#include "module_server.h"
#include "module_wifi.h"

void setup()
{
    Serial.begin(115200);

    // 初始化led
    initLED();

    // 初始化舵机
    initServo();

    // 初始化摄像头
    initCamera();

    // 配置wifi
    connectToWiFi(CONNECTTIMEOUT);

    // 初始化mqtt
    initMQTTConfig();

    // 初始化wedsocket服务
    initWedServer();

    // 创建视频流任务
    startCameraServerTask();
}
void loop()
{
    mqttLoop();
    if (!wifitate()) {
        checkDNS_HTTP();
    }
    checkConnect(true);
    vTaskDelay(5);
}
