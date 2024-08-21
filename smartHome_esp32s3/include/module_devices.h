#ifndef _MODULE_DEVICES_H_
#define _MODULE_DEVICES_H_
#include <Arduino.h>

// RGB_LED
#define RGBLEDPIN 48

#define MQ2PIN 8
#define DHTPIN 14
#define DHTTYPE DHT11

// #define PIN_R 2
// #define PIN_G 1
// #define PIN_B 38

// MAX98357
#define I2S_MAX_PORT I2S_NUM_0
// #define PIN_I2S_MAX98357_LRC 9
// #define PIN_I2S_MAX98357_BCLK 10
// #define PIN_I2S_MAX98357_DOUT 11
#define PIN_I2S_MAX98357_LRC 11
#define PIN_I2S_MAX98357_BCLK 10
#define PIN_I2S_MAX98357_DOUT 9

#define SAMPLE_RATE (16000)
#define CHUNK_SIZE 2048

// INMP411
#define I2S_NMP411_PORT I2S_NUM_1
#define PIN_I2S_INMP411_WS 47
#define PIN_I2S_INMP411_SD 45
#define PIN_I2S_INMP411_SCK 21

// #define BUTTON_PIN 13
// #define BUTTON_PIN1 14

#define RECORD_TIME_SECONDS 60
#define BUFFER_SIZE (SAMPLE_RATE * RECORD_TIME_SECONDS)

void initWSrgbled();
void WSLED_Red();
void WSLED_Green();
void WSLED_Blue();
void WSLED_OFF();
void redled_on();
void redled_off();
void greenled_on();
void greenled_off();
void blueled_on();
void blueled_off();
void rgbled_setColor(int r, int g, int b);

void initmq2();
float readmq2();

void initDHT();
float dhtReadTemperature();
float dhtReadHumidity();

void initDevices();
void startSensorTask(void);

#endif