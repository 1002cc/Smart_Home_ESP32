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

    // 初始化摄像头
    initCamera();

    // 配置wifi
    Serial.println("Connecting to WiFi");
    connectToWiFi(CONNECTTIMEOUT);

    // 初始化mqtt
    initMQTTConfig();

    // 初始化wedsocket服务
    initWedServer();
    // 开始视频流服务
    xTaskCreatePinnedToCore(cameraserver_task, "cameraserver_task", 5 * 1024, NULL, 5, NULL, 1);
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
