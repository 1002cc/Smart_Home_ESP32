#include "module_devices.h"
#include "module_mqtt.h"
#include "module_server.h"
#include "module_wifi.h"

lampButtonData mqttSwitchState = {false, false};

extern bool enable_pri;
extern bool enable_VoiceControl;
extern bool activeEnableAutoLamp;

int rainState = 0,
    soundState = 1, pirState = 0;

bool mqttVoice = false;
bool mqttPri = false;

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
    unsigned long autoLampTime = 0, rainTime = 0, priTime = 0, voiceTime = 0;
    bool rainFlag = 0, isRain = 0;
    while (1) {
        rainState = map(analogRead(RAIN_CHANNL), 0, 4095, 235, 0);
        if (rainState > 0) {
            if (!rainTime && !isRain) {
                rainTime = millis();
            }
        } else {
            rainTime = 0;
            isRain = false;
            if (rainFlag) {
                rainFlag = 0;
                pulishState("rainState", isRain);
            }
        }
        if (rainState && rainTime && millis() - rainTime >= RAINTIME) {
            isRain = true;
            rainTime = 0;
            rainFlag = 1;
            Serial.println("has rain");
            playAudio(AUDIO_NAME::RAIN);
            pulishState("rainState", isRain);
        }

        if (enable_pri) {
            pirState = analogRead(PIR_CHANNL);
            if (pirState > 0) {
                mqttPri = true;
                priTime = millis();
                pulishState("priState", true);
                Serial.println("has people");
            } else {
                if (mqttPri && millis() - priTime >= DETECTIONTIME) {
                    mqttPri = false;
                    pulishState("priState", false);
                }
            }
        } else {
            pirState = 0;
        }

        if (enable_VoiceControl) {
            soundState = digitalRead(SOUND_PIN);
            if (!soundState) {
                mqttVoice = true;
                voiceTime = millis();
                pulishState("voiceState", true);
                Serial.println("has sound");
            } else {
                if (mqttVoice && millis() - voiceTime >= DETECTIONTIME) {
                    mqttVoice = false;
                    pulishState("voiceState", false);
                }
            }
        } else {
            soundState = 1;
        }

        if (pirState > 0 || !soundState || activeEnableAutoLamp) {
            autoLampTime = millis();
            if (!autoled_state()) {
                autoled_on();
                mqttSwitchState.lampButton1 = true;
                pulishState("lampButton1", mqttSwitchState.lampButton1, "switches");
            }
        } else {
            if (autoled_state() && millis() - autoLampTime >= AUTOLAMPTIME) {
                autoled_off();
                mqttSwitchState.lampButton2 = false;
                pulishState("lampButton2", mqttSwitchState.lampButton1, "switches");
            }
        }

        if (millis() - lastPrintTime > 2000) {
            Serial.printf("rainState = %d  soundState = %d  pirState = %d\n", rainState, soundState, pirState);
            lastPrintTime = millis();
        }

        if (digitalRead(BUTTON_PIN1)) {
            delay(20);
            if (digitalRead(BUTTON_PIN1)) {
                Serial.println("BUTTON_PIN1");
                mqttSwitchState.lampButton1 = !led3w_state();
                mqttSwitchState.lampButton1 ? led3w_on() : led3w_off();
                pulishState("lampButton1", mqttSwitchState.lampButton1, "switches");
            }
        }

        if (digitalRead(BUTTON_PIN2)) {
            delay(20);
            if (digitalRead(BUTTON_PIN2)) {
                Serial.println("BUTTON_PIN2");
                mqttSwitchState.lampButton2 = !autoled_state();
                mqttSwitchState.lampButton2 ? autoled_on() : autoled_off();
                pulishState("lampButton2", mqttSwitchState.lampButton2, "switches");
            }
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