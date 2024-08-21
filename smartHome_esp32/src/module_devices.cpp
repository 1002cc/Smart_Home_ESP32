#include "module_devices.h"
#include "module_mqtt.h"
#include "module_wifi.h"
#include <ArduinoJson.h>
#include <Audio.h>
#include <HTTPClient.h>

Audio audio;
uint8_t cur_volume = 21;

lampButtonData mqttSwitchState = {false, false};

int rainState, soundState, pirState;

int pos = 0;

/********************************************************************
                         led1
*******************************************************************

void rgbled_init()
{
    pinMode(RPIN, OUTPUT);
    pinMode(GPIN, OUTPUT);
    pinMode(BPIN, OUTPUT);
}

void setColor(int r, int g, int b)
{
    analogWrite(RPIN, r);
    analogWrite(GPIN, g);
    analogWrite(BPIN, b);
}

void rgbled_red()
{
    setColor(255, 0, 0);
}

void rgbled_green()
{
    setColor(0, 255, 0);
}

void rgbled_blue()
{
    setColor(0, 0, 255);
}

void rgbled_off()
{
    setColor(0, 0, 0);
}

void rgbled_setColor(int r, int g, int b)
{
    setColor(r, g, b);
}

void redled_on()
{
    digitalWrite(PIN_R, 1);
}

void redled_off()
{
    digitalWrite(PIN_R, 0);
}

int redled_state()
{
    return digitalRead(PIN_R);
}

void greenled_on()
{
    digitalWrite(PIN_G, 1);
}

void greenled_off()
{
    digitalWrite(PIN_G, 0);
}

int greenled_state()
{
    return digitalRead(PIN_G);
}

void blueled_on()
{
    digitalWrite(PIN_B, 1);
}

void blueled_off()
{
    digitalWrite(PIN_B, 0);
}

int blueled_state()
{
    return digitalRead(PIN_B);
}

*/

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
                         audio
********************************************************************/

void audio_init()
{
    audio.setPinout(PIN_I2S_MAX98357_BCLK, PIN_I2S_MAX98357_LRC, PIN_I2S_MAX98357_DOUT);
    audio.setVolume(cur_volume);
    // audio.connecttohost(stations[cur_station].c_str());
}

void audioPause()
{
    audio.stopSong();
    Serial.println(audio.isRunning());
}

void audioTask(void *pt)
{
    Serial.println("start audioTask");
    while (1) {
        audio.loop();
        vTaskDelay(5);
    }
    vTaskDelete(NULL);
}

String getvAnswer(const String &ouputText)
{
    HTTPClient http2;
    http2.begin(MINIMAX_TTS);
    http2.addHeader("Content-Type", "application/json");
    http2.addHeader("Authorization", String("Bearer ") + MINIMAX_KEY);
    // 创建一个StaticJsonDocument对象，足够大以存储JSON数据
    StaticJsonDocument<200> doc;
    // 填充数据
    doc["text"] = ouputText;
    doc["model"] = "speech-01";
    doc["audio_sample_rate"] = 32000;
    doc["bitrate"] = 128000;
    doc["voice_id"] = "audiobook_female_1";
    // 创建一个String对象来存储序列化后的JSON字符串
    String jsonString;
    // 序列化JSON到String对象
    serializeJson(doc, jsonString);
    int httpResponseCode = http2.POST(jsonString);
    if (httpResponseCode == 200) {
        DynamicJsonDocument jsonDoc(1024);
        String response = http2.getString();
        Serial.println(response);
        http2.end();
        deserializeJson(jsonDoc, response);
        String aduiourl = jsonDoc["audio_file"];
        return aduiourl;
    } else {
        Serial.printf("tts %i \n", httpResponseCode);
        http2.end();
        return "error";
    }
}

void audioSpeak(const String &text)
{
    if (wifitate()) {
        String aduiourl = getvAnswer(text);
        Serial.println(aduiourl);
        if (aduiourl != "error") {
            audio.stopSong();
            audio.connecttohost(aduiourl.c_str());
        }
    }
}

void startAudioTack()
{
    xTaskCreatePinnedToCore(audioTask, "audio_task", 1024 * 3, NULL, 2, NULL, 1);
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