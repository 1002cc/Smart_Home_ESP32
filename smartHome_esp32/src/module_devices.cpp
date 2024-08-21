#include "module_devices.h"
#include "module_mqtt.h"
#include "module_server.h"
#include "module_wifi.h"

lampButtonData mqttSwitchState = {false, false};

int rainState, soundState, pirState;

int pos = 0;

/********************************************************************
                         LED
********************************************************************/

void rgbled_init()
{
    pinMode(AUTOLED_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(ONBOARDLAMPPIN, OUTPUT);
    led_off();
    autoled_off();
}

void led_on()
{
    digitalWrite(ONBOARDLAMPPIN, 1);
}
void led_off()
{
    digitalWrite(ONBOARDLAMPPIN, 0);
}

void blinkLED(int n, int t)
{
    for (int i = 0; i < 2 * n; i++) {
        digitalWrite(ONBOARDLAMPPIN, !digitalRead(ONBOARDLAMPPIN));
        delay(t);
    }
}

void autoled_on()
{
    digitalWrite(AUTOLED_PIN, 1);
}
void autoled_off()
{
    digitalWrite(AUTOLED_PIN, 0);
}

int autoled_state()
{
    return digitalRead(AUTOLED_PIN);
}

void led3w_on()
{
    digitalWrite(LED_PIN, 1);
}

void led3w_off()
{
    digitalWrite(LED_PIN, 0);
}

int led3w_state()
{
    return digitalRead(LED_PIN);
}

/********************************************************************
                         sg90
********************************************************************/

void sg90_init()
{
    pinMode(SG90_PIN, OUTPUT);
    ledcSetup(1, 50, 8);
    ledcAttachPin(SG90_PIN, 1);
}
void sg90_setAngle(int angle)
{
    ledcWrite(1, angle);
}

/********************************************************************
                         RAIN
********************************************************************/

void sensorTask(void *pt)
{
    unsigned long lastPrintTime = 0;
    unsigned long autoLampTime = 0, rainTime = 0;
    bool rainFlag = 0, isRain = 0;
    while (1) {
        rainState = map(analogRead(RAIN_CHANNL), 0, 4095, 235, 0);
        if (rainState > 0) {
            if (rainFlag) {
                rainTime = millis();
                rainFlag = 0;
            }
        } else {
            if (isRain) {
                isRain = false;
                // pulishState("rain", false);
            }
            rainFlag = 1;
        }
        // if (rainState && millis() - rainTime >= RAINTIME) {
        //     isRain = true;
        //     Serial.println("has rain");
        //     audioSpeak("下雨了,请注意关窗");
        //     pulishState("rain", isRain);
        // }

        // soundState = map(analogRead(SOUND_PIN), 0, 4095, 235, 0);
        soundState = digitalRead(SOUND_PIN);
        if (!soundState) {
            Serial.println("has sound");
        }

        pirState = analogRead(PIR_CHANNL);
        if (pirState > 0) {
            // autoLampTime = millis();
            // if (!autoled_state()) {
            //     autoled_on();
            //     // mqttSwitchState.lampButton2 = true;
            //     //  pulishState("lampButton4", mqttSwitchState.lampButton4, "switches");
            // }
            Serial.println("has people");
        } else {
            // if (autoled_state() && millis() - autoLampTime >= AUTOLAMPTIME) {
            //     autoled_off();
            //     mqttSwitchState.lampButton4 = false;
            //     // pulishState("lampButton4", mqttSwitchState.lampButton4, "switches");
            // }
        }

        if (millis() - lastPrintTime > 1000) // 每秒打印一次
        {
            Serial.printf("rainState = %d  soundState = %d  pirState = %d\n", rainState, soundState, pirState);
            lastPrintTime = millis();
        }

        if (digitalRead(BUTTON_PIN1)) {
            Serial.println("BUTTON_PIN1");
            mqttSwitchState.lampButton1 = !autoled_state();
            mqttSwitchState.lampButton1 ? autoled_on() : autoled_off();
            // pulishState("lampButton1", mqttSwitchState.lampButton1, "switches");
        }

        if (digitalRead(BUTTON_PIN2)) {

            Serial.println("BUTTON_PIN2");
            mqttSwitchState.lampButton2 = !led3w_state();
            mqttSwitchState.lampButton2 ? led3w_on() : led3w_off();
            // pulishState("lampButton2", mqttSwitchState.lampButton2, "switches");
        }

        vTaskDelay(200);
    }
    vTaskDelete(NULL);
}

void sensor_init()
{
    pinMode(SOUND_PIN, INPUT);

    pinMode(BUTTON_PIN1, INPUT);
    pinMode(BUTTON_PIN2, INPUT);
}

void startSensorTask()
{
    xTaskCreatePinnedToCore(sensorTask, "sensor_task", 1024 * 5, NULL, 2, NULL, 0);
}

/********************************************************************
                         initDevices
********************************************************************/
void initDevices()
{
    rgbled_init();
    sensor_init();
    sg90_init();
    audio_init();
}