#include "module_blue.h"
#include "module_devices.h"
#include "module_mqtt.h"
#include "module_server.h"
#include "module_wifi.h"

// 通信模式
ConnectionMode cMode = MODE;

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

    // 加载设备数据
    initDevicesDatas();

    // 开始传感器任务
    startSensorTask();

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
    if (cMode == ConnectionMode::WIFI || ConnectionMode::WB) {
        mqttLoop();
        if (!wifitate()) {
            checkDNS_HTTP();
        }
        checkConnect(true);
    } else if (cMode == ConnectionMode::BLUE) {
        // blueLoop();
    }

    vTaskDelay(500);
}
