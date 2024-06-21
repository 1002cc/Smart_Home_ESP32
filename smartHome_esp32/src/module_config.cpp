#include "module_config.h"

#include "WiFi.h"
#include "module_devices.h"
#include "module_service.h"
#include <Preferences.h>
#include <vector>

// Wifi配置
const char *ssid = "317";
const char *password = "317123456";

int networkTimeout = 10 * 1000;
extern bool hasNetwork;

Preferences preferences;

bool wifiConnect()
{
    WiFi.disconnect(true);
    Serial.println("preferences read wifi info");

    String storedSSID = preferences.getString("ssid", "");
    String storedPassword = preferences.getString("password", "");

    Serial.println("Connecting to ");
    Serial.print(ssid);
    Serial.print(" Password:");
    Serial.println(password);
    if (storedSSID != "" && storedPassword != "") {
        Serial.print("use flash wifi info : \nssid :");
        Serial.print(storedSSID);
        Serial.print(" Password:");
        Serial.println(storedPassword);

        ssid = storedSSID.c_str();
        password = storedPassword.c_str();
    }

    Serial.print(ssid);
    Serial.print(" Password:");
    Serial.println(password);
    WiFi.begin(ssid, password);

    unsigned long startingTime = millis();
    int ledstatus = 0;
    while (WiFi.status() != WL_CONNECTED && (millis() - startingTime) < networkTimeout) {
        ledstatus ? led_off() : led_on();
        ledstatus = !ledstatus;
        Serial.print(".");
        vTaskDelay(250);
    }
    vTaskDelay(1000);
    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\r\n-- wifi connect success! --\r\n");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        led_on();
        return true;
    }

    Serial.println("Connection to WiFi failed");
    led_off();
    return false;
}

void updateFlashDate()
{
    preferences.begin("wifi-settings", false);

#if USE_AUDIO
    if (preferences.getInt("volume", 1000) == 1000) { // if that: pref was never been initialized
        preferences.putInt("volume", 10);
        preferences.putInt("station", 0);
    } else {
        audioStation(preferences.getInt("station"));
        audioVolume(preferences.getInt("volume"));
    }
#endif
}
