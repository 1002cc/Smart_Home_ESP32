/*****************************/
/*****************************
 *  智能家居硬件控制开发框架
 *  @hich
 *  介绍:
 *  该项目为智能家居控制系统的硬件开发框架,
    主要为了让开发者更好的进行智能家居系统的开发和接入智能家居控制系统生态.
 *  功能:
    提供MQTT模块,配网模块
 *
 */

#include "module_mqtt.h"
#include "module_server.h"
#include "module_wifi.h"
#include <Arduino.h>

void setup()
{
    Serial.begin(115200);

    printPSRAM();
    // 配置wifi
    connectToWiFi(CONNECTTIMEOUT);

    // 初始化mqtt
    initMQTTConfig();
}

void loop()
{
}
