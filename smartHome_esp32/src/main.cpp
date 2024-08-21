#include "module_devices.h"
#include "module_mqtt.h"
#include "module_server.h"
#include "module_wifi.h"
#include <Audio.h>
#include <LittleFS.h>

extern Audio audio;
void setup()
{
    Serial.begin(115200);

    printPSRAM();

    // 初始化传感器设备
    initDevices();

    // 配置wifi
    Serial.println("Connecting to WiFi");
    // connectToWiFi(CONNECTTIMEOUT);

    // // 初始化mqtt
    // initMQTTConfig();

    startAudioTack();
    // startSensorTask();

    // if (wifitate()) {
    //     audioSpeak("连接成功");
    // }
    printPSRAM();

    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
    }
    Serial.println("LittleFS init succesful");
    audio.connecttoFS(LittleFS, "/conect_s.mp3");
}

void loop()
{
    // mqttLoop();
    // if (!wifitate()) {
    //     checkDNS_HTTP();
    // }
    // checkConnect(true);

    vTaskDelay(500);
}
