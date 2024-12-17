#include "module_devices.h"
#include "DHT.h"
#include "module_audio.h"
#include "module_mqtt.h"
#include "ui.h"
#include <FastLED.h>
#if USE_MQU
#include <MQUnifiedsensor.h>

#define Board ("ESP-32-S3")
#define Pin MQ2PIN
#define Type ("MQ-2")
#define Voltage_Resolution (3.3)
#define ADC_Bit_Resolution (12)
#define RatioMQ2CleanAir (9.83)
MQUnifiedsensor MQ2(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

#endif

DHT dht(DHTPIN, DHTTYPE);

CRGB leds[1];

/********************************************************************
                         WS2812BRGBLED
********************************************************************/
void initWSrgbled()
{
    FastLED.addLeds<WS2812B, RGBLEDPIN>(leds, 1);
}

void WSLED_Red()
{
    leds[0] = CRGB::Green;
    FastLED.show();
}
void WSLED_Green()
{
    leds[0] = CRGB::Red;
    FastLED.show();
}

void WSLED_Blue()
{
    leds[0] = CRGB::Blue;
    FastLED.show();
}
void WSLED_OFF()
{
    leds[0] = CRGB::Black;
    FastLED.show();
}

/********************************************************************
                         RGBled
*******************************************************************

void rgbled_init()
{
    pinMode(PIN_R, OUTPUT);
    pinMode(PIN_G, OUTPUT);
    pinMode(PIN_B, OUTPUT);
}

void setColor(int r, int g, int b)
{
    analogWrite(PIN_R, r);
    analogWrite(PIN_G, g);
    analogWrite(PIN_B, b);
}

void redled_on()
{
    digitalWrite(PIN_R, 1);
}

void redled_off()
{
    digitalWrite(PIN_R, 0);
}

void greenled_on()
{
    digitalWrite(PIN_G, 1);
}

void greenled_off()
{
    digitalWrite(PIN_G, 0);
}

void blueled_on()
{
    digitalWrite(PIN_B, 1);
}

void blueled_off()
{
    digitalWrite(PIN_B, 0);
}

void rgbled_setColor(int r, int g, int b)
{
    setColor(r, g, b);
}
*/

/********************************************************************
                         mq2
********************************************************************/

void initmq2()
{
#if USE_MQU
    MQ2.setRegressionMethod(1);
    MQ2.setA(574.25);
    MQ2.setB(-2.222);
    MQ2.init();
    Serial.println("Calibrating please wait.");
    float calcR0 = 0;
    for (int i = 1; i <= 10; i++) {
        MQ2.update();
        calcR0 += MQ2.calibrate(RatioMQ2CleanAir);
        Serial.print(".");
    }
    MQ2.setR0(calcR0 / 10);
    if (isinf(calcR0)) {
        Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
    }
    if (calcR0 == 0) {
        Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
    }
    Serial.println(" done!");
#endif
}
float readmq2()
{
    int MQ2_Sensor_Value = 0;
    int MQ2_Ouput_Value = 0;
#if USE_MQU
    MQ2.update();
    MQ2_Ouput_Value = MQ2.readSensor();
#else
    MQ2_Sensor_Value = analogRead(MQ2PIN);
    MQ2_Ouput_Value = map(MQ2_Sensor_Value, 0, 1023, 0, 100);
#endif
    return MQ2_Ouput_Value;
}

/********************************************************************
                         DHT11
********************************************************************/

void initDHT()
{
    dht.begin();
}
float dhtReadTemperature()
{
    return dht.readTemperature();
}

float dhtReadHumidity()
{
    return dht.readHumidity();
}

/********************************************************************
                         DevicesTask
********************************************************************/

void sensor_task(void *pvParameter)
{
    float temperature, humidity, mq2sensorValue;
    char temp_char[12];
    unsigned long int lastPrintTime, lastMQTime;
    int MQSure = 0;
    while (1) {
        temperature = dht.readTemperature();
        humidity = dht.readHumidity();
        mq2sensorValue = readmq2();

        if (isnan(temperature) || isnan(humidity)) {
            temperature = 0;
            humidity = 0;
            Serial.println("Failed to read from DHT sensor!");
        } else {
            lv_arc_set_value(ui_TemperatureArc, (int16_t)temperature);
            lv_arc_set_value(ui_HumidityArc, (int16_t)humidity);
            snprintf(temp_char, sizeof(temp_char), "%.0f°C", temperature);
            lv_label_set_text(ui_TemperatureLabel, temp_char);
            snprintf(temp_char, sizeof(temp_char), "%.0f%%", humidity);
            lv_label_set_text(ui_HumidityLabel, temp_char);
        }

        if (isnan(mq2sensorValue)) {
            mq2sensorValue = 0;
            Serial.println("Failed to read from MQ2 sensor!");
        } else {
            if (mq2sensorValue >= 30) {
                MQSure++;
                if (MQSure >= 3) {
                    MQSure = 0;
                    if (MQSure > 300) {
                        initmq2();
                        MQSure = 0;
                    }
                    if (millis() - lastMQTime > 6000) {
                        lastMQTime = millis();
                        playMQAlarm();
                    }
                }
            } else {
                MQSure = 0;
            }
            lv_arc_set_value(ui_MQArc, (int16_t)mq2sensorValue);
            snprintf(temp_char, sizeof(temp_char), "%.0f%%", mq2sensorValue);
            lv_label_set_text(ui_MQLabel, temp_char);
        }

        // if (millis() - lastPrintTime > 10000) {
        //     Serial.printf("Temperature: %.2f °C, Humidity: %.2f%%, mq2: %.2f%%\n", temperature, humidity, mq2sensorValue);
        //     lastPrintTime = millis();
        // }

        if (getMqttStart()) {
            SensorData sensorData = {
                .temp = temperature,
                .humidity = humidity,
                .mq = mq2sensorValue,
            };
            publishSensorData(sensorData);
        }
        vTaskDelay(4000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

/********************************************************************
                         initDevices
********************************************************************/

void initDevices()
{
    Serial.println("Init Devices ...");
    initWSrgbled();
    initDHT();
    initmq2();
    xTaskCreatePinnedToCore(sensor_task, "sensor_task", 3 * 1024, NULL, 5, NULL, 0);
    Serial.println("Init Devices Done");
}
