#ifndef _MODULE_DEVICES_H_
#define _MODULE_DEVICES_H_
#include <Arduino.h>

// RGB_LED
#define RGBLEDPIN 48

#define MQ2PIN 8
#define DHTPIN 7
#define DHTTYPE DHT11

#define PIN_R 2
#define PIN_G 1
#define PIN_B 38

// MAX98357
#define I2S_MAX_PORT I2S_NUM_0
#define PIN_I2S_MAX98357_LRC 9
#define PIN_I2S_MAX98357_BCLK 10
#define PIN_I2S_MAX98357_DOUT 11
#define SAMPLE_RATE (8000)

// INMP411
#define I2S_NMP411_PORT I2S_NUM_1
#define PIN_I2S_INMP411_WS 13
#define PIN_I2S_INMP411_SD 14
#define PIN_I2S_INMP411_SCK 21

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