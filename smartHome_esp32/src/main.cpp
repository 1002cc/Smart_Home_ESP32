#include "WiFi.h"
#include <esp_wifi.h>

#include "module_config.h"
#include "module_devices.h"
#include "module_service.h"
#include "smarthome_mqtt.h"

TimerHandle_t ntpTimer;
unsigned long lastMillis = 0;

bool hasNetwork = false;
extern bool palyState;

void WiFiEvent(WiFiEvent_t event)
{
    switch (event) {
    case SYSTEM_EVENT_STA_DISCONNECTED:
        hasNetwork = false;
        Serial.println("wifi connect no");
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        Serial.println("wifi connect ok");
        hasNetwork = isNetworkAvailable();
        Serial.printf("\r\n-- wifi connect success! --\r\n");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("isNetworkAvailable : ");
        Serial.println(hasNetwork);
        break;
    }
}

void setup()
{
    Serial.begin(115200);

    initDevices();
    updateFlashDate();
    delay(500);

    WiFi.onEvent(WiFiEvent);
    if (!wifiConnect()) {
        Serial.println("Connection to WiFi failed");
    } else {
        Serial.println("Connection to WiFi success");
    }
    delay(500);
    hasNetwork = isNetworkAvailable();
    if (hasNetwork) {
        Serial.println("NetworkAvailable is true");
        if (!initMQTT()) {
            Serial.println("MQTT connect failed");
        } else {
            Serial.println("MQTT connect success");
        }

        ntpTimer = xTimerCreate("NTP and Weather Timer", pdMS_TO_TICKS(3600000), pdTRUE, (void *)0, ntpTimerCallback);
        xTimerStart(ntpTimer, 0);
        ntpTimerCallback(NULL);
    } else {
        Serial.println("NetworkAvailable is false");
    }

#if USE_AUDIO
    startAudioTack();
#endif
}

void loop()
{
    if (hasNetwork) {
        mqttloop();
    }
}
