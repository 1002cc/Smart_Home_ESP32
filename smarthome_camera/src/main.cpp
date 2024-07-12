#include "esp_camera.h"
#include "module_camera.h"
#include "module_devices.h"
#include "module_mqtt.h"
#include "module_server.h"
#include "module_wifi.h"

void setup()
{
    Serial.begin(115200);
    Serial.println();

    initLED();

    initCamera();

    Serial.println("Connecting to WiFi...");
    connectToWiFi(CONNECTTIMEOUT);
    Serial.println("Connected to WiFi");

    if (!initMQTTConfig()) {
        Serial.println("MQTT init failed");
        return;
    }
}

void loop()
{
    mqttLoop();
    if (!wifitate()) {
        checkDNS_HTTP();
    }
    checkConnect(true);
}
