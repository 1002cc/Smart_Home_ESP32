#include "esp_camera.h"
#include "module_camera.h"
#include "module_mqtt.h"
#include "module_server.h"

#include <WiFi.h>

const char *ssid = "317";
const char *password = "317123456";

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    initCamera();

    WiFi.begin(ssid, password);
    WiFi.setSleep(false);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    initNtpTime();

    if (!initMQTTConfig()) {
        Serial.println("MQTT init failed");
        return;
    }

    Serial.print("Camera Ready! Use 'ip:");
    Serial.print(WiFi.localIP());
    Serial.println("to connect");
}

void loop()
{
    mqttLoop();
}
