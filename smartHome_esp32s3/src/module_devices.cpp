#include "module_devices.h"
#include "DHT.h"
#include "module_mqtt.h"
#include "ui.h"
#include <FastLED.h>
#include <MQUnifiedsensor.h>

DHT dht(DHTPIN, DHTTYPE);

#define Board ("ESP-32-S3")
#define Pin MQ2PIN
#define Type ("MQ-2")
#define Voltage_Resolution (3.3)
#define ADC_Bit_Resolution (12)
#define RatioMQ2CleanAir (9.83)
MQUnifiedsensor MQ2(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

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
********************************************************************/

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

/********************************************************************
                         mq2
********************************************************************/

void initmq2()
{
    MQ2.setRegressionMethod(1);
    MQ2.setA(987.99);
    MQ2.setB(-2.162);
    MQ2.init();
    Serial.print("Calibrating please wait.");
    float calcR0 = 0;
    for (int i = 1; i <= 10; i++) {
        MQ2.update(); // Update data, the arduino will read the voltage from the analog pin
        calcR0 += MQ2.calibrate(RatioMQ2CleanAir);
        Serial.print(".");
    }
    MQ2.setR0(calcR0 / 10);
    Serial.println("  done!.");

    if (isinf(calcR0)) {
        Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
        while (1)
            ;
    }
    if (calcR0 == 0) {
        Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
        while (1)
            ;
    }
}
float readmq2()
{
    MQ2.update();
    return MQ2.readSensor();
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
    initDHT();
    initmq2();
    float temperature, humidity, mq2sensorValue;
    char temp_char[12];
    while (1) {
        temperature = dhtReadTemperature();
        humidity = dhtReadHumidity();
        mq2sensorValue = readmq2();

        if (isnan(temperature) || isnan(humidity) || isnan(mq2sensorValue)) {
            Serial.println("Failed to read from DHT sensor!");
        } else {
            Serial.print("Temperature: ");
            Serial.print(temperature);
            Serial.print(" °C, Humidity:");
            Serial.print(humidity);
            Serial.print("% ");
            Serial.print(" mq2: ");
            Serial.println(mq2sensorValue);

            lv_arc_set_value(ui_TemperatureArc, (int16_t)temperature);
            lv_arc_set_value(ui_HumidityArc, (int16_t)humidity);
            snprintf(temp_char, sizeof(temp_char), "%.0f°C", temperature);
            lv_label_set_text(ui_TemperatureLabel, temp_char);
            snprintf(temp_char, sizeof(temp_char), "%.0f%%", humidity);
            lv_label_set_text(ui_HumidityLabel, temp_char);
            lv_arc_set_value(ui_MQArc, (int16_t)mq2sensorValue);
            snprintf(temp_char, sizeof(temp_char), "%d%%", (int)mq2sensorValue);
            lv_label_set_text(ui_MQLabel, temp_char);
            if (getMqttStart()) {
                SensorData sensorData = {
                    .temp = temperature,
                    .humidity = humidity,
                    .mq = mq2sensorValue,
                };
                publishSensorData(sensorData);
            }
            vTaskDelay(3000 / portTICK_PERIOD_MS);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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
    rgbled_init();
    Serial.println("Init Devices Done");
}

void startSensorTask(void)
{
    Serial.println("Starting sensor task");
    xTaskCreatePinnedToCore(sensor_task, "sensor_task", 4 * 1024, NULL, 5, NULL, 1);
}