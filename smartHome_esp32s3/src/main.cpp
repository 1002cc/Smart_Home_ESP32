#include "confighelpr.h"
#include "lvglconfig.h"
#include "module_audio.h"
#include "module_devices.h"
#include "module_mqtt.h"
#include "module_service.h"
#include "module_speak.h"
#include "wificonfig.h"
#include <Arduino.h>
#include <WiFi.h>
const char *ssid1 = "317";
const char *password1 = "317123456";
void setup()
{
    Serial.begin(115200);
    Serial.println("smarthome start-up");
    printPSRAM();
    initLVGLConfig();
    LittleFS_init();
    initDevices();
    initWIFIConfig();
    initMQTTConfig();
    lv_gohome();
    startNTPTask();
    initI2SConfig();
    initSpeakConfig();
    startSensorTask();
    printPSRAM();
#if USE_AUDIO
    startAudioTack();
#endif
}

void loop()
{
    mqttLoop();
    // speakloop();
}
