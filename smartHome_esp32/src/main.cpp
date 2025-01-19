#include "module_devices.h"
#include "module_mqtt.h"
#include "module_server.h"
#include "module_wifi.h"

/*
智能家居自动化联动
1.入侵检测
人体检测传感器,上位机开启入侵检测,当人体传感器检测到有人进入则触发语音报警(是否短信通知)
2.烟雾警报
烟雾传感器检测到烟雾或气体浓度超标后,触发语音报警和打开窗帘和窗户并开启风扇通风(是否短信通知)
3.雨滴检测
检测到雨滴后,语音报警和关闭窗户
4.门磁感应
门磁打开或关闭语音提示,超时间未关提示
*/

void setup()
{
    Serial.begin(115200);

    // 打印PSRAM
    printPSRAM();

    // 初始化文件系统
    littlefs_init();

    // 开始音频任务
    startAudioTack();

    // 初始化传感器设备
    initDevices();

    // 配置wifi
    connectToWiFi(CONNECTTIMEOUT);

    // 初始化mqtt
    initMQTTConfig();

    // 播放欢迎语
    playAudio(AUDIO_NAME::WC);

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
