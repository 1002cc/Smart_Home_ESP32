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
    initSpeakConfig();
    startSensorTask();
#if USE_AUDIO
    startAudioTack();
#endif
    printPSRAM();
}

void loop()
{
    mqttLoop();
    vTaskDelay(100 / portTICK_PERIOD_MS);
}
